<?php
/*
 * Extra functions for editing nodes called by hooks in the .module file
 */

function _txorg_taxonomy_term_presave($term) {
	if (isset($term->synonym_added) && $term->synonym_added === true) {
		return;
	}
	if (txorg_pattern_merge_enabled($term->vid)) {
		$matchCriteria = txorg_term_match_criteria($term->vid);
		$existingTerm = false;
		if (!isset($term->tid)) {
			$existingTerm = txorg_match_by_synonym($term);
		}
		if (!$existingTerm) {
			$existingTerm = txorg_match_term($term, $matchCriteria);
		}
		if ($existingTerm->tid > 0 && (!isset($term->tid) || $existingTerm->tid != $term->tid)) {
			$name = $term->name;
			$term = $existingTerm;
			if ($matchCriteria->storeSynonyms() ) {
				txorg_add_synonym($term,$name,LANGUAGE_NONE);
			}
			if (preg_match('#taxonomy/\w+/#',$_GET['q'])) {
				$translations = array('@term' => $term->name);
				$msg = t("A variant of this term @term already exists.",$translations);
				if ($matchCriteria->storeSynonyms()) {
					$msg .= " ". t("However, the variant was saved as a synonym");
				}
				drupal_set_message($msg);
			}
		}
	}
}

function txorg_fetch_term_by_name($term_name,$vid = 0,$save = false) {
		$term = txorg_match_term_name($term_name,$vid);
		if ($term->tid > 0 && $save && $term->name == $term_name) {
			$save = false;
		}
		if ($term->tid <1) {
			$term = entity_create('taxonomy_term', array('vid' => $vid,'name' => $term_name));
			$save = false;
		}
		if ($save) {
			taxonomy_term_save($term);
		}
		return $term;
}

function txorg_match_term_name($term_name,$vid = 0) {
	$term = (object) array('vid' => $vid,'name' => $term_name);
	if (txorg_pattern_merge_enabled($vid)) {
		$matchCriteria = txorg_term_match_criteria($vid);
		$term = txorg_match_term($term, $matchCriteria);
	} else {
		$comparisonString = preg_replace('#\s+#',' ',strtolower($term_name));
		$comparisonString = preg_replace("#'#","'?",$comparisonString);
		$term = txorg_match_pattern($term,$comparisonString);
	}
	return $term;
}


function txorg_match_term($term,VocabularyMatchCriteria $matchCriteria,$matchAllIds = false) {
	$simpleName = txorg_cleanstring($term->name, $matchCriteria);
	$matchCriteria->allowFuzzyEnd(true);
	$strFuzzy ='';
	$fuzzyRegex = '(e?s)';
	$fuzyMatchSingleWords = true;
	$comparisonString = str_replace($matchCriteria->separator(), ' ', $simpleName);
	$minFuzzyMatchLength = 2;
	if ($matchCriteria->allowFuzzyEnd()) {
		$strFuzzy = $fuzzyRegex.'?';
		$replRgx = '#'.$fuzzyRegex.'$#';
		if ($fuzyMatchSingleWords) {
			$words = explode(' ',(string) $comparisonString);
			foreach ($words as $index => $word) {
				if (strlen($word) > $minFuzzyMatchLength) {
					$words[$index] = preg_replace($replRgx,'',$word).$strFuzzy;
				}
			}
			$comparisonString = implode(' ',$words);
		} else if (strlen($comparisonString) > $minFuzzyMatchLength) {
			$comparisonString = preg_replace($replRgx,'',$comparisonString).$strFuzzy;
		}
	}
	if ($matchCriteria->translateAccents()) {
		$comparisonString = txorg_translate_accents($comparisonString,$term->vid);
	}
	$comparisonString = str_replace(' ',txorg_alphanumeric_sequence(false).'*', $comparisonString);
	return txorg_match_pattern($term,$comparisonString,$matchAllIds);
}

function txorg_match_pattern($term,$pattern,$matchAllIds = false,$mode='both',$boundsType = 'mysql') {
	if ($boundsType == 'mysql') {
		$endBoundary = '[[:>:]]';
		$startBoundary = '[[:<:]]';
	} else {
		$startBoundary = $endBoundary = '\b';
	}
	$endString = '('.txorg_alphanumeric_sequence(false).'*$|$)';
	$startString = '^'.txorg_alphanumeric_sequence(false).'*';
	$strWordSeparator = txorg_alphanumeric_sequence(false).'*';
	switch ($mode) {
		case 'start':
			$pattern = $startString.$pattern;
			break;
		case 'startwords':
			$pattern = $startString.$pattern.$endBoundary;
			break;
		case 'end':
			$pattern .= $endString;
			break;
		case 'endwords':
			$pattern = $startBoundary.$pattern.$endString;
			break;
		case 'contains':
			// unaltered
			break;
		case 'words':
			$pattern = $startBoundary.str_replace($strWordSeparator,$endBoundary.$strWordSeparator.$startBoundary,$pattern).$endBoundary;
			break;
		default:
			$pattern = $startString.$pattern.$endString;
			break;
	}
	$query = db_select('taxonomy_term_data','td')->fields('td',array('tid'));
	$query->condition('td.vid', $term->vid);
	if (isset($term->tid) && $term->tid > 0) {
		$query->condition('td.tid', $term->tid, '!=');
	}
	$query->condition('td.name', $pattern,'REGEXP');
	$result = $query->execute();
	if ($matchAllIds) {
		$tids = array(0);
	} else {
		$tid = 0;
	}
	if ($result) {
		if ($matchAllIds) {
			$tids = $result->fetchCol();
		} else {
			$tid = (int) $result->fetchField();
		}
	}
	if ($matchAllIds) {
		return $tids;
	}
	if ($tid > 0) {
		return taxonomy_term_load($tid);
	}
	return (object) array('tid' => 0, 'vid' => $term->vid);
}

function txorg_matches_term($currTerm,$term,VocabularyMatchCriteria $matchCriteria) {
	$matched = txorg_match_term($term,$matchCriteria);
	return $matched->tid == $currTerm->tid; 
}

function txorg_matched_term_ids($term,VocabularyMatchCriteria $matchCriteria) {
	return txorg_match_term($term,$matchCriteria,true);
}

function txorg_basic_string($string = NULL,$separator = '\s',$removeChars=false) {
	$charSequence = ';\.,\!":';
	$removableCharSequence = '\'`';
	$string = strtolower(trim($string));
	if ($removeChars) {
		$string = preg_replace('#['.$removableCharSequence.']+#','',$string);
	} else {
		$charSequence .= $removableCharSequence;
	}
	// replace with single space
	if ($separator == '\s') {
		$separator = ' ';
	}
	return trim(preg_replace('#['.$charSequence.']+#',$separator,$string));
}

function txorg_cleanstring($string = NULL, VocabularyMatchCriteria $matchCriteria) {
	$separator = $matchCriteria->separator();
	// remove these punctuation marks
	$string = txorg_basic_string($string);
	if ($matchCriteria->removeMinorWords() ) {
		$minorWords = txorg_minor_words( $matchCriteria->langCode() );
		$string = preg_replace('#\b('.implode('|',$minorWords).')\b#','',$string);
	}
	$string = preg_replace('#'.txorg_alphanumeric_sequence(false).'+#',$separator,$string);
	// replace space with default separator for comparison
	$string = preg_replace(array('#'.$separator.'+?$#','#^'.$separator.'+?#'),'',$string);
	return $string;
}

/*
 * Translate UTF-8 accented characters for comparison purposes only
 * e.g. café and cafe or Zurich and Zürich would be considered synonyms
 */
function txorg_translate_accents($string = NULL,$vocabId = 0,$langcode = 'en') {
	$equivalents = txorg_accent_map($vocabId);
	$charSets = array();
	foreach ($equivalents as $letter => $equiv) {
		if (!array_key_exists($equiv,$charSets)) {
			$charSets[$equiv] = array($equiv);
		}
		$charSets[$equiv][] = $letter; 
	}
	foreach ($charSets as $target => $cSet) {
		$replaceRgx = '('.implode('|', $cSet).')';
		$string = preg_replace('#'.$replaceRgx.'#i',$replaceRgx,$string);
	}
	return $string;
}

function txorg_accent_map($vocabId = 0,$langcode='en') {
	$data = variable_get('txorg_accent_equivalents_'.$langcode.'_'.$vocabId, NULL);
	if (empty($data) || !is_array($data)) {
		$varKey = 'txorg_accent_equivalents_'.$langcode.'_0';
		$data = variable_get($varKey, NULL);
		if (empty($data)) {
			$data = txorg_accent_map_default();
			variable_set($varKey, $data);
		}
	}
	return $data;
}

function txorg_alphanumeric_sequence($positive = true) {
	$strNegate = $positive === false? '^' : '';
	return '['.$strNegate.'a-z0-9'.txorg_get_modified_letter_list().']';
}

function txorg_get_modified_letter_list() {
	return implode(array_keys(txorg_accent_map_default()));
}

function txorg_accent_map_default() {
	return array(
			'á' => 'a',
			'é' => 'e',
			'é' => 'e',
			'ó' => 'o',
			'ú' => 'u',
			'à' => 'a',
			'è' => 'e',
			'ì' => 'i',
			'ò' => 'o',
			'ù' => 'u',
			'ä' => 'a',
			'ë' => 'e',
			'ï' => 'i',
			'ö' => 'o',
			'ü' => 'u',
			'â' => 'a',
			'ê' => 'e',
			'î' => 'i',
			'ộ' => 'o',
			'ü' => 'u',
			'å' => 'a',
			'æ' => 'ae',
			'ɨ' => 'i',
			'ø' => 'o',
			'ß' => 'ss',
	);
}

function txorg_minor_words_default() {
	return array(
		'the',
		'a',
		'an',
	);
}

function txorg_minor_words($vid = 0, $langCode = 'en') {
	$default = txorg_minor_words_default();
	return variable_get('txorg_minor_words_' . $langCode .'_'.$vid, $default);
}

function txorg_term_match_criteria($vid = 0,$langcode = NULL) {
	$none = array();
	module_load_include('php','txorg','includes/VocabularyMatchCriteria');
	if (empty($langcode)) {
		$langcode = txorg_default_lang();
	}
	$options = variable_get('txorg_vocabulary_term_match_criteria_' . $langcode. '_' . $vid , $none);
	return new VocabularyMatchCriteria($options);
}

function txorg_add_synonym($term,$variant = NULL, $langCode = 'und') {
	if (is_string($variant)) {
		$variant = trim($variant);
		$items = field_get_items('taxonomy_term', $term, TXORG_SYNONYMS_FIELD_NAME);
		$items = is_array($items) ? $items : array();
		if (!empty($items)) {
			foreach ($items as $item) {
				if (isset($item['value']) && strtolower(trim($item['value'])) == strtolower($variant)) {
					return false;
				}
			}
		}
		$items[] = array('value' => $variant);
		$term->{TXORG_SYNONYMS_FIELD_NAME}[$langCode] = $items;
		$term->synonym_added = true;
		taxonomy_term_save($term);
	}
	return false;
}

function txorg_transfer_form($form,&$form_state,$object = NULL) {
	$form = array();
	if (is_object($object) && isset($object->vid)) {
		
		if (arg(2) == 'taxonomy') {
			$term = (object) array('tid' => 0, 'vid' => $object->vid, 'name' => '');
			$vocab = $object;
			$termMode = false;
		} else {
			$term = $object;
			$vocab = taxonomy_vocabulary_load($term->vid);
			drupal_set_title( t("Reassign terms to") . ": " . $term->name );
			$termMode = true;
		}
		
		$vid = $object->vid;
		$hasPattern = (isset($_GET['pattern']) && strlen($_GET['pattern']) > 2);
		if ($hasPattern) {
			$mode = $_GET['mode'];
			$pattern = strtolower(trim(urldecode($_GET['pattern'])));
		} else {
			$mode = 'both';
			$pattern = txorg_basic_string($term->name);
		}
		$fields = txorg_term_reference_fields($vid);
		$numMatches = 0;
		$addPager = false;
		$unlinkedTids = array();
		$fieldInfo = implode(', ',$fields);
		$strInfo = '';
		if ($termMode) {
			$itemData = txorg_term_details($term);
			$strInfo = $itemData->table;
		}

		
		$languages = language_list();
		$defLangcode = 'en';
		$defLanguageName = t("English");
		$langOpts=array($defLangcode => $defLanguageName);
		if (!empty($languages)) {
			foreach ($languages as $langcode => $language) {
				if ($language->enabled ==1 && !isset($langOpts[$langcode])) {
					$langOpts[$langcode] = $language->name;
				}
			}
		}
		
		if (count($langOpts)> 1) {
			$langcodeField = array(
					'#type' => 'select',
					'#title' => t("Language"),
					'#options' => $langOpts,
					'#default_value' => $_SESSION['txorg']['langcode']
			);
		} else {
			$langcodeField = array(
					'#type' => 'hidden',
					'#value' => $defLangcode
			);
		}
		$form['langcode'] = $langcodeField;
		
		$tree = taxonomy_get_tree($term->vid);
		
		$data = txorg_build_term_table_options($tree,$term,$termMode,$fields,$matchAll,'transfer');
		$editAlias = 'admin/structure/taxonomy/%s/taxonomy-organiser';
		$translations = array(
				'@num' => $numItems,
				'@terms' => format_plural($numItems,'term','terms'),
				'@termName' => $term->name,
				'@termId' => $term->tid,
				'@vocabName' => $vocab->name,
				'@numMatched' => $numMatches,
				'@match' => format_plural($numMatches,'matches','match'),
				'@be' => format_plural($numMatches,'is','are'),
		);
		if ($termMode) {
			$editVocab = l('(Organise terms in '.$vocab->name.')',sprintf($editAlias,$vocab->machine_name));
		} else {
			$editVocab = '';
		}
		if ($termMode) {
			$matchInfo = $editVocab .", " . t("@matchPhrase @match @termName",$translations);
		} else {
			$matchInfo = '';
		}
		$listingInfo = "<p>" .t("@num @terms @be in this vocabulary",$translations)." ".$matchInfo."</p>";
		
		if (!empty($strInfo)) {
			$form['info'] = array(
					'#type' => 'fieldset',
					'#title' => t("Associated Entities") . " (".$itemData->num_items.")",
					'#description' => $strInfo,
					'#collapsible' => TRUE,
					'#collapsed' => !isset($_GET['page']),
					'#weight' => -1
			);
		}
		if ($addPager) {
			pager_default_initialize($numItems, $maxShownByDefault, 0);
			$pager = theme('pager');
		} else {
			$pager = '';
		}
		
		$tableLabel = t('Terms in @vocabName',$translations);
		$form['listing'] = array(
				'#type' => 'fieldset',
				'#title' => $tableLabel,
				'#description' => $listingInfo,
		);
		
		if ($data->total > 0) {
			$form['listing']['terms'] = array(
					'#type' => 'tableselect',
					'#header' => $data->header,
					'#options' => $data->rows,
					'#default_value' => 0,
					'#multiple' => FALSE,
					'#prefix' => $pager,
					'#suffix' => $pager,
			);
		}
	}
	$form['listing']['actions'] = array(
			'#type' => 'fieldset',
			
	);
	$form['actions']['merge'] = array(
			'#type' => 'submit',
			'#id' => 'edit-transfer',
			'#value' => t('Transfer to selected term'),
			'#suffix' => " ". t("Reassign all term reference for @termName [@termId] into the term selected above",$translations)
	);
	$form['term'] = array(
			'#type' => 'hidden',
			'#value' => $term
	);
	// 
	$form['unlinked_tids'] = array(
			'#type' => 'hidden',
			'#value' => $unlinkedTids
	);
	$form['referer'] = array(
			'#type' => 'hidden',
			'#value' => $_GET['q']
	);
	$form['#submit'] = array('txorg_transfer_form_submit');
	//txorg_reset_messages();
	return $form;
}

function txorg_merge_form($form,&$form_state,$object = NULL) {
	$form = array();
	if (is_object($object) && isset($object->vid)) {

		if (arg(2) == 'taxonomy') {
			$term = (object) array('tid' => 0, 'vid' => $object->vid, 'name' => '');
			$vocab = $object;
			$termMode = false;
		} else {
			$term = $object;
			$vocab = taxonomy_vocabulary_load($term->vid);
			drupal_set_title( t("Reassign terms to") . ": " . $term->name );
			$termMode = true;
		}

		$vid = $object->vid;
		$hasPattern = (isset($_GET['pattern']) && strlen($_GET['pattern']) > 2);
		if ($hasPattern) {
			$mode = $_GET['mode'];
			$pattern = strtolower(trim(urldecode($_GET['pattern'])));
		} else {
			$mode = 'both';
			$pattern = txorg_basic_string($term->name);
		}
		$fields = txorg_term_reference_fields($vid);
		$numMatches = 0;
		$addPager = false;
		$unlinkedTids = array();
		$fieldInfo = implode(', ',$fields);
		$strInfo = '';
		if ($termMode) {
			$itemData = txorg_term_details($term);
			$strInfo = $itemData->table;
		}


		$languages = language_list();
		$defLangcode = 'en';
		$defLanguageName = t("English");
		$langOpts=array($defLangcode => $defLanguageName);
		if (!empty($languages)) {
			foreach ($languages as $langcode => $language) {
				if ($language->enabled ==1 && !isset($langOpts[$langcode])) {
					$langOpts[$langcode] = $language->name;
				}
			}
		}

		if (count($langOpts)> 1) {
			$langcodeField = array(
					'#type' => 'select',
					'#title' => t("Language"),
					'#options' => $langOpts,
					'#default_value' => $_SESSION['txorg']['langcode']
			);
		} else {
			$langcodeField = array(
					'#type' => 'hidden',
					'#value' => $defLangcode
			);
		}
		$form['langcode'] = $langcodeField;

		$form['match'] = array(
				'#type' => 'fieldset',
				'#title' => 'Match by pattern',
				'#tree' => true,
				'#collapsible' => true,
				'#collapsed' => !$hasPattern,
		);

		$form['match']['string'] = array(
				'#type' => 'textfield',
				'#title' => 'Filter by terms matching this pattern',
				'#default_value' => $pattern,
		);

		$opts = array(
				'start' => 'Beginning with characters',
				'startwords' => 'Beginning with whole words',
				'end' => 'Ending with characters',
				'endwords' => 'Ending with whole words',
				'contains' => 'Containing characters',
				'words' => 'Containing whole word(s)',
				'both' => 'Match whole string',
		);

		$form['match']['mode'] = array(
				'#type' => 'select',
				'#title' => 'Match mode',
				'#options' => $opts,
				'#default_value' => $mode,
		);

		$form['match']['string_info'] = array(
				'#type' => 'fieldset',
				'#title' => t("Pattern matching instructions"),
				'#description' => txorg_pattern_matching_help(),
				'#collapsible' => true,
				'#collapsed' => true,
		);

		$form['match']['filter'] = array(
				'#type' => 'submit',
				'#value' => 'Filter',
				'#id' => 'edit-match-filter',
		);
		if (!$hasPattern) {
			$tree = taxonomy_get_tree($term->vid);
			$matchAll = false;
		} else {
			$rgxPattern = txorg_translate_regex_pattern($pattern, $vid);
			$matchedTids = txorg_match_pattern($term,$rgxPattern,true,$mode);
			$tree = array();
			if (!empty($matchedTids)) {
				$tree = taxonomy_term_load_multiple($matchedTids);
				$tree = array_values($tree);
			}
			$matchAll = true;
		}
		$data = txorg_build_term_table_options($tree,$term,$termMode,$fields,$matchAll, 'merge');
		$editAlias = 'admin/structure/taxonomy/%s/taxonomy-organiser';
		$translations = array(
				'@num' => $numItems,
				'@terms' => format_plural($numItems,'term','terms'),
				'@termName' => $term->name,
				'@termId' => $term->tid,
				'@vocabName' => $vocab->name,
				'@numMatched' => $numMatches,
				'@match' => format_plural($numMatches,'matches','match'),
				'@be' => format_plural($numMatches,'is','are'),
		);
		if ($termMode) {
			if ($numMatches > 0) {
				$translations['@matchPhrase'] = t('of which @numMatched', $translations);
			} else {
				$translations['@matchPhrase'] = 'none of which';
			}
			$editVocab = l('(Organise terms in '.$vocab->name.')',sprintf($editAlias,$vocab->machine_name));
		} else {
			$editVocab = '';
		}
		if ($hasPattern) {
			$listingInfo = "<p>" .t("@num @terms in this vocabulary @match the string",$translations).": <em>".$pattern."</em> ".$editVocab ."</p>";
		} else {
			if ($termMode) {
				$matchInfo = $editVocab .", " . t("@matchPhrase @match @termName",$translations);
			} else {
				$matchInfo = '';
			}
			$listingInfo = "<p>" .t("@num @terms @be in this vocabulary",$translations)." ".$matchInfo."</p>";
		}
		$showListing = ($hasPattern || $numMatches > 0);
		$strNoMatches = $showListing? "" : " (" . t("No matches found") . ")";

		if (!empty($strInfo)) {
			$form['info'] = array(
					'#type' => 'fieldset',
					'#title' => t("Associated Entities") . " (".$itemData->num_items.")",
					'#description' => $strInfo,
					'#collapsible' => TRUE,
					'#collapsed' => (isset($_GET['page']) || $showListing),
					'#weight' => -1
			);
		}
		if ($addPager) {
			pager_default_initialize($numItems, $maxShownByDefault, 0);
			$pager = theme('pager');
		} else {
			$pager = '';
		}
		if ($termMode) {
			$tableLabel = t('Other terms in this vocabulary to be merged with @termName',$translations);
		} else {
			$tableLabel = t('Terms in @vocabName',$translations);
		}
		$form['listing'] = array(
				'#type' => 'fieldset',
				'#collapsible' => TRUE,
				'#title' => $tableLabel.$strNoMatches,
				'#description' => $listingInfo,
				'#collapsed' => !$showListing
		);
		if ($data->total > 0) {
			$form['listing']['terms'] = array(
					'#type' => 'tableselect',
					'#header' => $data->header,
					'#options' => $data->rows,
					'#multiple' => TRUE,
					'#default_value' => $data->defaultValues,
					'#prefix' => $pager,
					'#suffix' => $pager,
			);
		}
	}
	$form['listing']['actions'] = array(
			'#type' => 'fieldset',
				
	);
	if ($termMode) {
		$form['actions']['merge'] = array(
				'#type' => 'submit',
				'#id' => 'edit-merge',
				'#value' => t('Merge selected'),
				'#suffix' => " ". t("into @termName [@termId]",$translations)
		);
	} else {
		$form['actions']['delte_all'] = array(
				'#type' => 'submit',
				'#id' => 'edit-delete-all',
				'#value' => 'Delete all unassociated terms',
		);
	}
	$form['term'] = array(
			'#type' => 'hidden',
			'#value' => $term
	);
	//
	$form['unlinked_tids'] = array(
			'#type' => 'hidden',
			'#value' => $unlinkedTids
	);
	$form['referer'] = array(
			'#type' => 'hidden',
			'#value' => $_GET['q']
	);
	$form['#submit'] = array('txorg_merge_form_submit');
	//txorg_reset_messages();
	return $form;
}


function txorg_translate_regex_pattern($pattern,$vid = 0) {
	$rgxPattern = str_replace('.*','%',$pattern);
	$rgxPattern = txorg_translate_accents($rgxPattern,$vid);
	$rgxPattern = str_replace('\w*',txorg_alphanumeric_sequence(true).'*',$rgxPattern);
	$rgxPattern = str_replace('*',txorg_alphanumeric_sequence(true).'*',$rgxPattern);
	$rgxPattern = str_replace('%','.*',$rgxPattern);
	return str_replace(' ',txorg_alphanumeric_sequence(false).'*', $rgxPattern);
}

function txorg_build_term_table_options(array &$tree,$term,$termMode = false,&$fields,$matchAll=false,$detailsCol = 'merge') {
	$data = new StdClass;
	$data->rows = array();
	$matchCriteria = txorg_term_match_criteria($term->vid);
	$taxonomyBase = 'taxonomy/term/';
	$editStr = $taxonomyBase . '%s/edit';
	$mergeStr = $taxonomyBase . '%s/reassign';
	$transferStr = $taxonomyBase . '%s/reassign';
	if ($detailsCol == 'transfer') {
		$detailsColHead = t('Transfer');
		$detailsTooltip = t("Assign all items associated with this term as a synonym of another term");
	} else {
		$detailsColHead = t('Merge by pattern');
		$detailsTooltip = t("Merge other terms into this term by pattern");
	}
	
	$data->header = array(
			'tid' => 'Term ID',
			'name' => 'Term name',
			'instances' => 'Freq.',
			'details' => $detailsColHead,
			'edit' => 'Edit',
	);
	$data->total = 0;
	if (!empty($tree)) {
		$data->total = count($tree);
		if (!$matchAll) {
			$checkedOpts = txorg_matched_term_ids($term,$matchCriteria);
			$checkedOpts = array_flip($checkedOpts);
			$numMatchingTerms = count($checkedOpts);
		} else {
			$numMatchingTerms = $data->total;
		}
		$data->defaultValues = array();
	
		$showUnMatched = false;
		$showAll = (isset($_GET['show']) && $_GET['show'] == 'all' || 'all' == arg(4));
		$maxShownByDefault = variable_get('txorg_max_display_all_merge_terms',TXORG_MAX_DISPLAY_ALL_MERGE_TERMS);
		$page = isset($_GET['page'])? $_GET['page'] : 0;
		$startIndex = $page * $maxShownByDefault;
		if ($numMatchingTerms < 1) {
			$showUnMatched = true;
		}
		else if ($data->total > $maxShownByDefault) {
			$showUnMatched = $showAll;
		}
		$addPager = (!$showAll && $data->total > $maxShownByDefault);
		$numMatches = 0;
		$index = 0;
		if ($showAll || !$showUnMatched || $hasPattern) {
			$startIndex = 0;
			$maxIndex = $data->total;
		} else if ($addPager) {
			$maxIndex = $maxShownByDefault +  $startIndex;
			if ($maxIndex > $data->total) {
				$maxIndex = $data->total;
			}
		} else {
			$maxIndex = $data->total;
		}
		
		$detailsOpts = array(
				'attributes' => array('title' => $detailsTooltip )
		);
		$editOpts = array(
				'attributes' => array('title' => t("Edit this term") )
		);
		for ($i = $startIndex; $i < $maxIndex; $i++) {
			if (isset($tree[$i]) && is_object($tree[$i]) && $tree[$i]->tid != $term->tid){
				if ($matchAll === true || isset($checkedOpts[$tree[$i]->tid])) {
					$matched = true;
					$numMatches++;
					$classes = array('preselected');
					$data->defaultValues[$tree[$i]->tid] = true;
				} else {
					$matched = false;
					$classes = array('unselected');
					$data->defaultValues[$tree[$i]->tid] = false;
				}
				if ($numMatchingTerms <1 && $index >= $maxShownByDefault) {
					break;
				}
				if ($matched || $showUnMatched) {
					$numMatched = 0;
					$matchStrings = array();
					if (!empty($fields)) {
						foreach ($fields as $fN) {
							$num = (int) txorg_term_field_count_matches($fN,$tree[$i]->tid);
							if ($num > 0) {
								$numMatched += $num;
							}
						}
					}
					if ($numMatched < 1) {
						$unlinkedTids[] = $tree[$i]->tid;
					}
					$row = array('tid' => $tree[$i]->tid, 'name' => $tree[$i]->name);
					$row['instances'] = $numMatched;
					if ($detailsCol == 'transfer') {
						$row['details'] = l('[' . t("transfer") . ']',sprintf($transferStr,$tree[$i]->tid),$detailsOpts);
					} else {
						$row['details'] = l('[' . t("merge") . ']',sprintf($mergeStr,$tree[$i]->tid),$detailsOpts);
					}
					$row['edit'] = l('['. t("edit") .']',sprintf($editStr,$tree[$i]->tid),$editOpts);
					$row['#attributes'] = array('class' => $classes);
					$data->rows[$tree[$i]->tid] = $row;
					$index++;
				}
			}
		}
	}
	return $data;
}

function txorg_pattern_matching_help() {
	$strHelp = "<p>".t('You may use simple regular expression syntax here.')."</p>";
	$strHelp .= "<ul>";
	$strHelp .= "<li>Control characters for the start and end of lines and word boundaries will be added automatically depending on the selected match mode  above. Do not use  <strong>^</strong> and <strong>$</strong> or <strong>\b</strong> for these purposes.</li>";
	$strHelp .= "<li><strong>%</strong> will match any characters, spaces or punctuation until the next character. (in true regular expressions this is <strong>.*</strong>, which you should not use here).</li>";
	$strHelp .= "<li><strong>*</strong> will match any alphanumeric characters in a word. (in true regular expressions this is <strong>(\w*)</strong>, which you should not use here)</li>";
	$strHelp .= "<li>For alternative character sequences, wrap pipe-separated strings in parentheses e.g. <strong>(lorry|truck)</strong> will match both <strong>lorry</strong> and <strong>truck</strong></li>";
	$strHelp .= "<li>Optional characters or strings may be denoted by <strong>?</strong>, if more than one character is optional, wrap it in parentheses e.g. <strong>girls?</strong> will match both <strong>girl</strong> and <strong>girls</strong>, <strong>meet(ing)?</strong> will match either <strong>meet</strong> or <strong>meeting</strong>, while <strong>pe(rsons?|ople)</strong> will match <strong>person</strong>, <strong>persons</strong> or <strong>people</strong></li>";
	$strHelp .= "<li>Character sets e.g. <strong>m[aou]m</strong> will match <strong>mam</strong>, <strong>mum</strong> and <strong>mom</strong></li>";
	$strHelp .= "<li>Character ranges such as [0-9] for all numerals or [a-z] for all letters from a to z (do not use \d or \w).</li>";
	$strHelp .= "<li>When matching whole words, all words must be separated by one or more space or punctuation mark. e.g. <strong>house-boat</strong>, <strong>house. boat</strong> and <strong>house boat</strong> would match <strong>house boat</strong></li>";
	$strHelp .= "<li>When matching character strings, spaces and punctuation marks will be ignored e.g. <strong>houseboat</strong>, <strong>house-boat</strong> and <strong>house boat</strong> would all match <strong>house boat</strong></li>";
	$strHelp .= "</ul>";
	return $strHelp;
}

function txorg_merge_form_submit($form,&$form_state) {
	if (isset($form_state['clicked_button']) && isset($form['term'])) {
		$values = $form_state['values'];
		$validTerm = $form['term']['#value'];
		switch ($form_state['clicked_button']['#id']) {
			case 'edit-merge':
				$terms = $values['terms'];
				if (is_array($terms)) {
					$tids = array();
					foreach ($terms as $key => $value) {
						if (is_numeric($key) && $key == $value && $value > 0) {
							$tids[] = (int) $value;
						} 
					}
					txorg_term_merge($validTerm,$tids);
					if (isset($values['referer']) && strlen($values['referer'])>5) {
						drupal_goto('taxonomy/term/'.$validTerm->tid.'/reassign');
					}
				}
				break;
			case 'edit-delete-all':
				$values = $form_state['values'];
				$validTerm = $form['term']['#value'];
				$tids = $values['unlinked_tids'];
				if (!empty($tids) && is_array($tids)) {
					txorg_term_delete_bulk($validTerm,$tids);
				}
				break;
			case 'edit-match-filter':
				$match = $values['match'];
				$pattern = $match['string']; 
				$mode = $match['mode'];
				txorg_term_merge_by_pattern($validTerm,$pattern,$mode);
				break;
		}
	}
}

function txorg_transfer_form_submit($form,&$form_state) {
	$hasValidTerm = false;
	if (isset($form_state['clicked_button']) && isset($form['term'])) {
		$values = $form_state['values'];
		$term = $form['term']['#value'];
		switch ($form_state['clicked_button']['#id']) {
			case 'edit-transfer':
				$tid = $values['terms'];
				if (is_numeric($tid) && $tid > 0) {
					$validTerm = taxonomy_term_load($tid);
					if (is_object($validTerm)) {
						txorg_term_merge($validTerm,$term->tid);
						$hasValidTerm = true;
					}
				}
				break;
		}
	}
	if ($hasValidTerm) {
		drupal_goto('taxonomy/term/'.$validTerm->tid.'/reassign');
	}
}

function txorg_term_delete_bulk($term,$tids) {
	$numDeleted = 0;
	if (is_object($term) && isset($term->vid)) {
		$vocab = taxonomy_vocabulary_load($term->vid);
		if (!empty($tids) && is_array($tids)) {
			foreach ($tids as $tid) {
				if (is_numeric($tid) && $tid > 0) {
					taxonomy_term_delete($tid);
					$numDeleted++;
				}
			}
		}
	}
	drupal_set_message(t("@num terms deleted from @vocab",array('@num' => $numDeleted,'@vocab' => $vocab->name)));
}

function txorg_term_details($term) {
	$data = (object) array('table' => '', 'num_items' => 0);
	if (is_object($term) && isset($term->tid)) {
		$matchCriteria = txorg_term_match_criteria($term->vid);
		$vocab = taxonomy_vocabulary_load($term->vid);
		$termReferenceFields = txorg_term_reference_fields($term->vid);
		$rows = array();
		$editAlias = 'admin/structure/taxonomy/'.$vocab->machine_name;
		$opts = array(
				'query' => array('destination' => $_GET['q'])
		);
		$strInfo  = txorg_list_synonyms($term);
		$header = array('Entity type','Bundle','Title','Active','Edit Entity');
		$nodeTypeNames = node_type_get_names();
		foreach ($termReferenceFields as $index => $fN) {
			$items = txorg_term_field_matches($fN,$term->tid);
			if (!empty($items)) {
				$data->num_items = count($items);
				foreach ($items as $item) {
					$cells = array();
					$hash = (array) $item;
					$strType = txorg_translate_machine_name($item->entity_type);
					$bundleName = txorg_translate_bundle_name($item->bundle,$item->entity_type);
					$cells[] = $strType;
					$cells[] = $bundleName;
					$entities = entity_load($item->entity_type,array($item->entity_id));
					$title = '';
					$strPublished = "";
					if (!empty($entities) && is_array($entities)) {
						$entity = array_shift($entities);
						if (is_object($entity)) {
							switch ($item->entity_type) {
								case 'user':
									$title = $entity->name;
									break;
								default:
									$title = $entity->title;
									break;
							}
							$strPublished = (isset($entity->status) && $entity->status > 0)? 'yes' : 'no';
						}
					}
					$cells[] = $title;
					$cells[] = $strPublished;
					switch ($item->entity_type) {
						case 'commerce_product':
							$editStr = 'admin/commerce/products/%s/edit';
							break;
						default:
							$editStr = $item->entity_type.'/%s/edit';
							break;
					}
					$cells[] = l('[edit]', sprintf($editStr,$item->entity_id));
					$rows[] = $cells;
				}
			}
		}
		if (empty($rows)) {
			return txorg_term_reference_field_info($termReferenceFields,$vocab );
		} else {
			$attrs = array('id' => 'taxonomy-organiser');
			$vars = array(
					'caption' => t("Entities referencing this term"),
					'attributes' => $attrs,
					'header' => $header,
					'rows' => $rows,
					'colgroups' => array(),
					'sticky' => FALSE,
					'empty' => FALSE
			);
			$data->table = theme('table',$vars).$strInfo;
		}
	}
	return $data;
}

function txorg_fields_form($form,&$form_state,$vocab = NULL) {
	$form = array();
	$field_name = arg(6);
	if (!empty($field_name) && is_string($field_name)) {
		$entity_type = arg(7);
		$bundle = arg(8);
    $tid = arg(9);
    if (empty($tid) || !is_numeric($tid)) {
      $tid = 0;
    } else {
      $tid = (int) $tid;
    }
		$mode = arg(10);
    if (empty($mode)) {
      $field = txorg_field_data($field_name);
      $mode = txorg_is_multiple($field)? 'checkboxes' : 'radios'; 
    }
		return txorg_field_instance_form($form,$vocab, $field_name,$entity_type,$bundle,$tid,$mode);
	} else {
		return _txorg_fields_form($form,$form_state,$vocab,$field_name);
	}
	
}

function _txorg_fields_form(&$form,&$form_state,$vocab = NULL,$field_name=NULL) {
	$form = array();
	if (is_object($vocab) && isset($vocab->vid)) {
		$vid = $vocab->vid;
		$numOptions = txorg_num_term_options($vid);
		$fields = txorg_term_reference_fields($vid);
		$fieldData = txorg_field_data($fields);
		$fieldListStr = 'admin/structure/taxonomy/'.$vocab->machine_name.'/taxonomy-organiser/fields/%s/%s/%s';
		$fieldEditStr = 'admin/structure/types/manage/%s/fields/%s';
		$commerceFieldEditStr = 'admin/commerce/products/types/%s/fields/%';
		$taxonomyFieldEditStr = 'admin/structure/taxonomy/%s/fields/%s';
		$header = array();
		$rows = array();
		$editTip = array('attributes' => array('title' => ''));
		foreach ($fieldData as $fieldName => $data) {
			if (isset($data['instances']) && is_array($data['instances'])) {
				foreach ($data['instances'] as $instance) {
					$cells = array();
					$cells[] = $instance['label'];
					$cells[] = txorg_translate_machine_name($instance['entity_type']);
					switch ($instance['entity_type']) {
						case 'node':
							$editStr = sprintf($fieldEditStr,$instance['bundle'],$instance['field_name']);
							break;
						case 'commerce_product':
							$editStr = sprintf($commerceFieldEditStr,$instance['bundle'],$instance['field_name']);
							break;
						case 'taxonomy_term':
							$editStr = sprintf($taxonomyFieldEditStr,$instance['bundle'],$instance['field_name']);
							break;
						default:
							$editStr = NULL;
							break;
					}
					$cells[] = l(txorg_translate_bundle_name($instance['bundle']),$editStr);
					$cells[] = $instance['required']==1? 'yes' : 'no';
						
					$cells[] = txorg_translate_cardinality($data['cardinality']);
					$cells[] = txorg_term_field_count_matches($fieldName,$instance['bundle'],$instance['entity_type']);
					$cells[] = txorg_num_term_options($vid,$fieldName,$instance['entity_type'],$instance['bundle']) . ' / ' . $numOptions;
					$editPath = sprintf($fieldListStr,$fieldName,$instance['entity_type'],$instance['bundle']);
					$control = txorg_match_widget($instance);
					$editTip['attributes']['title'] = "Edit with: " . $control;
					$editCell = l('[organise]',$editPath,$editTip);
					if ($numOptions <= TXORG_MAX_OPTIONS_EDITABLE_WITH_SELECTBOXES && $control != 'radios' && $control != 'checkboxes') {
						if (txorg_is_multiple($data)) {
							$secondControl = 'checkboxes';
						} else {
							$secondControl = 'radios';
						}
						$editTip['attributes']['title'] = "Edit with: " . $secondControl;
						$editCell .= "<br />" . l('[organise with '.$secondControl.']',$editPath.'/0/'.$secondControl,$editTip);
					}
					$cells[] = $editCell;
					
					$rows[] = $cells;
				}
			}
		}
	
		$vars['header'] = array('Field Name','Enity Type','Bundle','Required','Cardinality','No. of instances','No. of terms','edit');
		$vars['rows'] = $rows;
		$vars['attributes'] = array();
		$vars['caption'] = '';
		$vars['colgroups'] = array();
		$vars['sticky'] = false;
		$vars['empty'] = '';
	
		$form['field_info'] = array(
				'#type' => 'markup',
				'#markup' => theme_table($vars),
		);
	}
	$form['#submit'] = array('txorg_fields_form_submit');
	//txorg_reset_messages();
	return $form;
}


function txorg_num_term_options($vid = 0,$field_name=NULL,$entity_type=NULL,$bundle=NULL) {
	$num = 0;
	$dataTable = 'field_data_' . $field_name;
	$query = db_select('taxonomy_term_data','td')->fields('td',array('tid'));
	if (!empty($field_name)) {
		$query->join($dataTable,'fd',"td.tid=fd.".$field_name."_tid");
	}
	$query->condition('td.vid',$vid);
	if (!empty($field_name)) {
		if (!empty($entity_type)) {
			$query->condition('fd.entity_type',$entity_type);
		}
		if (!empty($bundle)) {
			$query->condition('fd.bundle',$bundle);
		}
		$query->groupBy('td.tid');
	}
	
	$result = $query->execute();
	if ($result) {
		$data = $result->fetchCol();
		if (!empty($data)) {
			$num = count($data);
		}
	}
	return $num;
}

function txorg_translate_cardinality($cardinality = 0) {
	switch ($cardinality) {
		case -1: case 0:
			$cardinalityText = t('unlimited');
			break;
		case 1:
			$cardinalityText = t('single');
			break;
		default:
			$cardinalityText = $cardinality . '(max)';
			break;
	}
	return $cardinalityText;
}

function txorg_term_merge_by_pattern($validTerm,$pattern = NULL,$mode = 'both') {
	$qOpts = array(
			'pattern'=>urlencode($pattern),
			'mode' => $mode
	);
	if (is_object($validTerm) && isset($validTerm->tid) && $validTerm->tid > 0) {
		$path = 'taxonomy/term/'.$validTerm->tid.'/reassign/';
	} else {
		$vocab = taxonomy_vocabulary_load($validTerm->vid);
		$path = 'admin/structure/taxonomy/'.$vocab->machine_name.'/taxonomy-organiser/reassign';
	}
	return drupal_goto($path,array('query' => $qOpts));
}

function txorg_term_merge($validTerm, $deprecatedTermId = NULL) {
	if (is_array($deprecatedTermId)) {
		$termIds = $deprecatedTermId;
		$deprecatedTermId = isset($termIds[0])? $termIds[0] : 0;
	} else {
		$termIds = array($deprecatedTermId);
	}
	$valid = (
			(is_object($validTerm) && isset($validTerm->vid) && isset($validTerm->tid)) &&
			$validTerm->vid > 0 &&
			$validTerm->tid > 0 &&
			(is_numeric($deprecatedTermId) && $deprecatedTermId > 0) &&
			!empty($termIds)
	);
	if ($valid) {
		$matchCriteria = txorg_term_match_criteria($validTerm->vid);
		$deprecatedTerm = taxonomy_term_load($deprecatedTermId);
		$deprecatedName = $deprecatedTerm->name;
		$termReferenceFields = txorg_term_reference_fields($validTerm->vid);
		if (!empty($termReferenceFields)) {
			foreach ($termReferenceFields as $field) {
				$data_table = 'field_data_'.$field;
				$rev_table = 'field_revision_'.$field;
				$tid_field = $field.'_tid';
				$fields = array(
					$tid_field => $validTerm->tid
				);
				$condition = db_and()->condition($tid_field,$termIds,'IN');
				$update = db_update($data_table)->fields($fields)->condition($condition)->execute();
				$update = db_update($rev_table)->fields($fields)->condition($condition)->execute();
			}
			if ($matchCriteria->storeSynonyms() ) {
				foreach ($termIds as $deprecatedTid) {
					$deprecatedTerm = taxonomy_term_load($deprecatedTid);
					if (is_object($deprecatedTerm) && isset($deprecatedTerm->name)) {
						txorg_add_synonym($validTerm,$deprecatedTerm->name,LANGUAGE_NONE);
					}
				}
			}
		}
		$numDeleted=0;
		foreach ($termIds as $tid) {
			taxonomy_term_delete($tid);
			$numDeleted++;
		}
		$translations = array(
				'@num' => $numDeleted,
				'@synonyms' => format_plural($numDeleted, 'synonym', 'synonyms'),
				'@validTerm' => $validTerm->name 
		);
		drupal_set_message(t("@num @synonyms were reassigned to @validTerm",$translations) );
	}
}

function txorg_term_field_matches($fieldName,$tid = 0, $entity_type=NULL) {
	return txorg_term_field_count_matches($fieldName,$tid,$entity_type, true);
}

function txorg_term_field_count_matches($fieldName=NULL,$tid = 0,$entity_type=NULL,$details = false) {
	$data = array();
	if ($details) {
		$fields = array('entity_id','entity_type','bundle','delta');
	} else {
		$fields = array('entity_id');
	}
	if (is_string($tid) && !is_numeric($tid)) {
		$bundle = $tid;
		if (strpos($bundle,'/') > 0) {
			list($b,$t) = explode('/',$bundle);
			if (is_numeric($t)) {
				$tid = (int) $t;
			}
			$bundle = $b;
		}
	} else if (is_numeric($tid)) {
		$bundle = NULL;
		$tid = (int) $tid;
	}
	if ($tid>0) {
		$comparator = '=';
	} else {
		$comparator = '>';
	}
	if (!empty($entity_type)) {
		$info = entity_get_info($entity_type);
		$entityTable = $info['base table'];
		$entity_pk = $info['entity keys']['id'];
	}
	
	$data_table = 'field_data_'.$fieldName;
	if (!empty($entity_type)) {
		$query = db_select($entityTable,'e')->fields('e',array($entity_pk));
		$query->leftJoin($data_table,'dt',"e.".$entity_pk ."=dt.entity_id");
	} else {
		$query  = db_select($data_table,'dt');
	}
	$query->fields('dt',$fields);
	if ($details) {
		$query->addExpression('GROUP_CONCAT(dt.'.$fieldName.'_tid)','tids');
	}
	if (!empty($bundle)) {
		$query->condition('dt.bundle',$bundle);
	}
	if (!empty($entity_type)) {
		$query->condition('dt.entity_type',$entity_type);
	}
	$query->condition($fieldName.'_tid',$tid,$comparator)
		->groupBy('dt.entity_id');
		$result =  $query->execute();
	if ($result) {
		if ($details) {
			$data = $result->fetchAll();
		} else {
			$data = $result->fetchCol();
		}
		if (!is_array($data)) {
			$data = array();
		}
	}
	$query->orderBy('dt.entity_id');
	if ($details) {
		foreach ($data as $index => $item) {
			if (isset($item->tids) && !empty($item->tids)) {
				$data[$index]->tids = explode(',',$item->tids);
			} else {
				$data[$index]->tids = array();
			}
		}
		return $data;
	} else {
		return count($data);
	}
}


function txorg_field_instance_form(&$form, $vocab,$field_name,$entity_type = NULL, $bundle = NULL,$tid = 0,$mode=NULL) {
	$tableOpts = array();
	$taxonomyBase = 'taxonomy/term/';
	$editStr = $taxonomyBase . '%s/edit';
	$detailsStr = $taxonomyBase . '%s/reassign';
	$header = array(
			'id' => 'Entity ID',
			'title' => 'Title',
			'edit' => 'Edit',
	);
	$numItems = 0;
	$tree = taxonomy_get_tree($vocab->vid);
	$numOptions = is_array($tree)? count($tree) : 0;
	$options = array();
	foreach ($tree as $id => $term) {
		$options[$term->tid] = $term->name;
	}
	$bundleRef = $bundle;
	if (is_numeric($tid)) {
		$tid = (int) $tid;
		$term = taxonomy_term_load($tid);
		if (!is_object($term)) {
			$tid = 0;
		}
	}
	
	if ($tid > 0) {
		$bundleRef .= '/' . $tid;
	}
	
	if (count($options) < 50) {
		$filterOpts = array('-' => '-- All --') + $options;
		$form['filter'] = array(
				'#type' => 'fieldset',
				'#tree' => FALSE,
				'#attributes' => array('class' => array('horizontal'))
		);
		$form['filter']['terms'] = array(
				'#type' => 'select',
				'#title' => 'Filter By',
				'#options' => 	$filterOpts,
				'#default_value' => $tid
		);
		$form['filter']['action'] = array(
				'#type' => 'submit',
				'#id' => 'edit-filter',
				'#value' => 	t("Filter"),
		);
	}
	
	$data = txorg_term_field_matches($field_name,$bundleRef,$entity_type);
	$perPage = 100;
	if (!empty($data)) {
		$numItems = count($data);
		$page = isset($_GET['page'])? $_GET['page'] : 0;
		$startIndex = $page * $perPage;
		$addPager = ($numItems > $perPage);
		$numMatches = 0;
		$index = 0;
		$maxIndex = $perPage +  $startIndex;
		if ($maxIndex > $numItems) {
			$maxIndex = $numItems;
		}
		$entityIds = array();
		for ($i = $startIndex; $i < $maxIndex; $i++) {
			if (isset($data[$i]) && is_object($data[$i]) && isset($data[$i]->entity_id)) {
				$entityIds[] = $data[$i]->entity_id;
			}
		}
		$entities = !empty($entityIds)? entity_load($entity_type,$entityIds) : array();
		pager_default_initialize($numItems, $perPage, 0);
		$pager = theme('pager');
		
		$field = txorg_field_data($field_name);

		foreach ($field['instances'] as $instanceData) {
			if ($instanceData['bundle'] == $bundle && $instanceData['entity_type'] == $entity_type) {
				$instance = $instanceData;
				break;
			}
		}
		$instanceInfo = txorg_instance_info($field,$instance);
		
		$multiple = txorg_is_multiple($field);

		$control = '';
		if (!empty($mode)) {
			switch ($mode) {
				case 'checkboxes':
					if ($multiple && $numOptions <= TXORG_MAX_OPTIONS_EDITABLE_WITH_SELECTBOXES) {
						$control = $mode;
					}
					break;
				case 'radios':
					if (!$multiple && $numOptions <= TXORG_MAX_OPTIONS_EDITABLE_WITH_SELECTBOXES) {
						$control = $mode;
					}
					break;
			}
		}
		if (empty($control)) {
			$control = txorg_match_widget($instance);
		}
		if ($entity_type == 'node') {
			$n_types = node_type_get_names();
			$bundleName = $n_types[$bundle];
		} else {
			$bundleName = ucwords(str_replace('_',' ',$bundle));
		}
		$entityName = ucwords(str_replace('_',' ',$entity_type));
		
		$caption =  $entityName." &gt; " . $bundleName . " &gt; " . $instance['label'] . " (" . $vocab->name . ")";
		if ($tid > 0) {
			$caption .= " &gt; " . $term->name;
		}
		$form['fields'] = array(
				'#type' => 'fieldset',
				'#name' => 'fields',
				'#tree' => TRUE,
				'#prefix' => $pager,
				'#suffix' => $pager,
				'#theme' => 'txorg_table',
				'#caption' => $caption
		);
		
		for ($i = $startIndex; $i < $maxIndex; $i++) {
			if (isset($data[$i]) && is_object($data[$i]) && isset($data[$i]->entity_id)) {
				$item = $data[$i];
				$classes = array();
				$entity = $entities[$item->entity_id];
        $term_items = field_get_items($entity_type,$entity,$field_name);
        if (!empty($term_items)) {
          $item->tids = array();
          foreach ($term_items as $ti) {
            $item->tids[] = $ti['tid'];
          }
        }
				switch ($control) {
					case 'select':
					case 'checkboxes':
					case 'radios':
						$default = $item->tids;
						break;
					default:
						$defaults = array();
						foreach ($item->tids as $tid) {
							$term = taxonomy_term_load($tid);
							if ($term) {
								$defaults[] = $term->name;
							}
						}
						$default = implode("\n",$defaults);
						break;
				}
				switch ($entity_type) {
					case 'node':
						$entity->title_link = l($entity->title, 'node/'.$entity->nid.'/edit');
						break;
					default:
						$entity->title_link = $entity->title;
						break;
				}
				$form['fields'][$item->entity_id] = array(
						'#type' => $control,
						'#title' => $entity->title_link,
						'#default_value' => $default,
				);
				
				
				switch ($control) {
					case 'select': case 'checkboxes': case 'radios':
						$form['fields'][$item->entity_id]['#options'] = $options;
						if ($multiple) {
							$form['fields'][$item->entity_id]['#multiple'] = true;
						}
						break;
				}
			}
	
			$form['field_name'] = array(
					'#type' => 'hidden',
					'#value' => $field_name,
			);
			
			$form['vid'] = array(
					'#type' => 'hidden',
					'#value' => $vocab->vid,
			);
			
			$form['machine_name'] = array(
					'#type' => 'hidden',
					'#value' => $vocab->machine_name,
			);
			
			$form['entity_type'] = array(
					'#type' => 'hidden',
					'#value' => $entity_type,
			);
			
			$form['bundle'] = array(
					'#type' => 'hidden',
					'#value' => $bundle,
			);
			
			$form['required'] = array(
					'#type' => 'hidden',
					'#value' => $instance['required'],
			);
			
			$form['widget'] = array(
					'#type' => 'hidden',
					'#value' => $instance['widget']['type'],
			);
			
			$form['control'] = array(
					'#type' => 'hidden',
					'#value' => $control,
			);
			
			$form['cardinality'] = array(
					'#type' => 'hidden',
					'#value' => $field['cardinality'],
			);
      
			$form['multiple'] = array(
					'#type' => 'hidden',
					'#value' => (int) $multiple,
			);
      
      
			$form['page'] = array(
					'#type' => 'hidden',
					'#value' => (int) $page,
			);
			
			$form['destination'] = array(
					'#type' => 'hidden',
					'#value' => $_GET['q'],
			);
			
			$form['save'] = array(
					'#type' => 'submit',
					'#value' => 'Save',
					'#id' => 'edit-save',
			);
		}
	}
  $form['#submit'] = array('txorg_field_instance_form_submit'); 
	return $form;
}


function txorg_match_widget($instance) {
	switch ($instance['widget']['type']) {
		case 'options_select':
			$control = 'select';
			break;
		case 'options_buttons':
			if ($multiple) {
				$control = 'checkboxes';
			} else {
				$control = 'radios';
			}
			break;
		case 'taxonomy_autocomplete':
			$control = 'textarea';
			break;
		default:
			$control = 'textfield';
			break;
	}
	return $control;
}

function txorg_is_multiple($field) {
	switch ($field['cardinality']) {
		case 1:
			$multiple = false;
			break;
		default:
			$multiple = true;
			break;
	}
	return $multiple;
}

function txorg_instance_info($field,$instance) {
	$table = array();
	$table = array(
			'caption' => t("Entity Types referencing this term"),
			'colgroups' => array(),
			'sticky' => FALSE,
			'empty' => FALSE
	);
	$table['header'] = array('Bundle','Entity type','Cardinality','Widget');
	$table['rows'] = array();
	$cells = array();
	
	$cells[] = txorg_translate_bundle_name($instance['bundle']);
	$cells[] = txorg_translate_machine_name($instance['entity_type']);
	$cells[] = txorg_translate_cardinality($field['cardinality']);
	$cells[] = txorg_field_widget_name($instance['widget']);
	$table['rows'][] = $cells;
	return theme('table',$table);
}

function txorg_field_widget_name($widget = array()) {
	$widget_name = $widget['type'];
	if (is_string($widget_name)) {
		$info = field_info_widget_types($widget_name);
		if (isset($info['label'])) {
			$widget_name = $info['label']; 
		}
	}
	return $widget_name;
}

function txorg_field_instance_form_submit($form,&$form_state) {
	$values = $form_state['values'];
  if (!isset($values['page'])) {
    $values['page'] = 0;
  }
  $opts = array('query' => array('page' => $values['page']));
	switch ($form_state['clicked_button']['#id']) {
		case 'edit-filter':
			if (isset($form_state['values']['terms'])) {
				$tid = $form_state['values']['terms'];
				if (is_numeric($tid) && $tid != '-') {
					$tid = (int) $tid;
				} else {
					$tid = 0;
				}
				list($path,$pathOpts) = explode('/taxonomy-organiser/',$_GET['q']);
				$redirect = $path . '/taxonomy-organiser';
				$pOpts = explode('/',$pathOpts);
				$numParts = count($pOpts);
				for ($i=0; $i < 6;$i++) {
					if ($i != 4) {
						if (isset($pOpts[$i])) {
							$redirect .= '/'.$pOpts[$i];
						} else {
							$redirect .= '/'.$tid;
						}
					}
				}
				drupal_goto($redirect,$opts);
			}
			break;
	}
	if (isset($values['entity_type']) && isset($values['fields'])) {
		$entity_type = $values['entity_type'];
		$bundle = $values['bundle'];
		$field_name = $values['field_name'];
		$control = $values['control'];
		$vid = $values['vid'];
		$fields = $values['fields'];
    $multiple = (bool) $values['multiple'];
		foreach ($fields as $entity_id => $termRef) {
			$result = entity_load($entity_type,array($entity_id));
			if (!empty($result) && is_array($result) && isset($result[$entity_id])) {
				$entity = $result[$entity_id];
				$checkName = false;
				$tids = array();
				if (is_scalar($termRef)) {
					switch ($control) {
						case 'textfield':
							$term = txorg_fetch_term_by_name($termRef,$vid,true);
							$tids = array($term->tid);
							break;
						case 'textarea':
							$term_names = explode("\n",$termRef);
							foreach ($term_names as $tname) {
								if (is_string($tname)) {
									$tname = trim($tname);
									if (!empty($tname)) {
										$term = txorg_fetch_term_by_name($tname,$vid,true);
										$tids[] = $term->tid;
									}
								}
							}
							break;
						case 'select':
						case 'checkboxex':
						case 'radios':
							$tids = array($termRef);
							break;
					}
				} else {
					$tids = $termRef;
				}
				$items = array();
				foreach ($tids as $tindex =>  $tid) {
					if (is_numeric($tid)) {
            if ($multiple) {
              if ($tindex > 0 && $tindex == $tid) {
                $items[] = array('tid' => $tid);
              }
            }
            else {
              if ($tid > 0) {
                $items[] = array('tid' => $tid);
              }
            }
					}
				}
				if (!isset($entity->language) || strlen($entity->language) < 2) {
					$entity->language = LANGUAGE_NONE;
				}
				$newMap = array(
						$entity->language => $items
				); 
				if (isset($entity->{$field_name}) && $entity->{$field_name} != $newMap) {
					$entity->{$field_name} = $newMap;
					entity_save($entity_type,$entity);
				} 
			}
		}
	}
	drupal_goto($values['destination'],$opts);
}


function txorg_term_reference_fields($vocabularyId = 0) {
	$fieldMap = field_info_fields();
	$data = array();
	$vocabulary = taxonomy_vocabulary_load($vocabularyId);
	if (is_object($vocabulary) && is_array($fieldMap)) {
		foreach ($fieldMap as $fieldName => $fData) {
			if ($fData['type'] == 'taxonomy_term_reference') {
				if (isset($fData['settings']['allowed_values']) && is_array($fData['settings']['allowed_values'])) {
					foreach ($fData['settings']['allowed_values'] as $allowedVal) {
						if ($allowedVal['vocabulary'] == $vocabulary->machine_name) {
							$data[] = $fData['field_name'];
						}
					}
				}

			}
		}
	}
	return $data;
}

function txorg_list_synonyms($term) {
	$synonyms = txorg_get_synonyms($term);
	$strOut = '';
	if (!empty($synonyms) && is_array($synonyms)) {
		$vars['items'] = $synonyms;
		$vars['title'] = t("Synonyms").":";
		$vars['type'] = 'ul';
		$strOut = theme('item_list',$vars);
	}
	return $strOut;
}

function txorg_get_synonyms($term) {
	$synonyms = field_get_items('taxonomy_term',$term,TXORG_SYNONYMS_FIELD_NAME);
	$items = array();
	if (!empty($synonyms) && is_array($synonyms)) {
		foreach ($synonyms as $syn) {
			$items[] = $syn['value'];
		}
	}
	return $items;
}

function txorg_match_by_synonym($term) {
	$matchedTerm = false;
	if (is_object($term) && isset($term->vocabulary_machine_name)) {
		$fieldTable = 'field_data_'.TXORG_SYNONYMS_FIELD_NAME;
    if (db_table_exists($fieldTable)) {
  		$valueField = TXORG_SYNONYMS_FIELD_NAME.'_value';
  		$query = db_select($fieldTable,'sn')->fields('sn',array('entity_id'));
  		$query->condition('sn.'.$valueField,$term->name,'LIKE');
  		$query->condition('sn.entity_type','taxonomy_term');
  		$query->condition('sn.bundle',$term->vocabulary_machine_name);
  		$result = $query->execute();
  		if ($result) {
  			$matchedId = $result->fetchField();
  			if (is_numeric($matchedId) && $matchedId>0) {
  				$matchedTerm = taxonomy_term_load($matchedId);
  			}
  		}
    }
	}
	return $matchedTerm;
}

function txorg_term_reference_field_info(&$termReferenceFields,$vocab) {
	$strInfo = '';
	$editAlias = 'admin/structure/taxonomy/'.$vocab->machine_name;
	$opts['attributes'] = array('title' => 'List terms in this vocabulary');
	$fieldData = txorg_field_data($termReferenceFields);
	$vars['title'] = t("Field instances");
	$vars['type'] = 'ul';
	$vars['items'] = array();
	if (!empty($fieldData)) {
		foreach ($fieldData as $field => $data) {
			foreach ($data['instances'] as $instance) {
				$vars['items'][] = $instance['label'] . " in " . txorg_translate_bundle_name($instance['bundle']) . " (".txorg_translate_machine_name($instance['entity_type']).") ";
			}
		}
	}
	return theme('item_list',$vars);;
}


function txorg_field_data($termReferenceFields = NULL) {
	$fieldData = array();
	$singleMode = false;
	if (!empty($termReferenceFields)) {
		if (!is_array($termReferenceFields)) {
			if (is_string($termReferenceFields)) {
				$termReferenceFields = array($termReferenceFields);
				$singleMode = true;
			} else {
				$termReferenceFields = array();
			}
		}
		$instances = field_info_instances();
		foreach ($termReferenceFields as $field) {
			foreach ($instances as $entityType => $entityFieldData) {
				foreach ($entityFieldData as $bundle => $bundleFieldData) {
					if (isset($bundleFieldData[$field])) {
						if (!isset($fieldData[$field])) {
							$fieldData[$field] = field_info_field($field);
							$fieldData[$field]['instances'] = array();
						}
						$fieldData[$field]['instances'][] = $bundleFieldData[$field];
					}
				}
			}
		}
	}
	if ($singleMode) {
		return isset($fieldData[$field])? $fieldData[$field] : array();
	}
	return $fieldData;
}

function txorg_translate_machine_name($machineName = NULL) {
	return ucwords(str_replace('_',' ',$machineName));
}

function txorg_translate_bundle_name($bundle=NULL,$entityType = NULL) {
	$bundleName = NULL;
	switch ($entityType) {
		case 'node':
			$bundleName = isset($nodeTypeNames[$bundle]) ? $nodeTypeNames[$bundle] : NULL;
			break;
	}
	if (empty($bundleName)) {
		$bundleName = txorg_translate_machine_name($bundle);
	}
	return $bundleName;
}

function _get_query_string(SelectQueryInterface $query) {
	$string = (string) $query;
	$arguments = $query->arguments();

	if (!empty($arguments) && is_array($arguments)) {
		foreach ($arguments as $placeholder => &$value) {
			if (is_string($value)) {
				$value = "'$value'";
			}
		}

		$string = strtr($string, $arguments);
	}

	return preg_replace('#[\{\}]#','',$string);
}