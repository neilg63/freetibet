<?php

function txorg_settings($vocabulary) {
	module_load_include('inc', 'txorg','txorg.edit');
	$form = drupal_get_form('txorg_settings_form',$vocabulary);
	return render($form);
}

function txorg_settings_form($form,&$form_state,$vocabulary = NULL) {
	txorg_reset_messages();
	$hasVocab = (is_object($vocabulary) && isset($vocabulary->vid) && isset($vocabulary->name));
	$form = array();
	if ($hasVocab) {
		$translations = array(
				'@vocab' => $vocabulary->name
		);
		drupal_set_title( t("Taxonomy organiser settings for @vocab",$translations) );
		$enableLabel = t('enable for this vocabulary');
		$vocabId = $vocabulary->vid;
	} else {
		drupal_set_title( t("Global taxonomy organiser settings") );
		$enableLabel = t('enable for all vocabularies');
		$vocabId = 0;
	}
	$default = txorg_settings_form_default();
	$settings = txorg_term_match_criteria($vocabId)->map();
	$langOpts = array();
	$defaultLangName = t('English');
	$languages = language_list();
	if (!empty($languages)) {
		foreach ($languages as $langcode => $lang) {
			if ($lang->enabled == 1) {
				$langOpts[$langcode] = $lang->name;
				if ($langcode == $settings['langcode']) {
					$defaultLangName = $lang->name;
				}
			}
		}
	}
	$enabled = (bool) variable_get('txorg_pattern_merge_' . $vocabId,true);
	
	$form['vid'] = array(
			'#type' => 'hidden',
			'#value' => $vocabId
	);
	
	$form['enable'] = array(
			'#type' => 'checkbox',
			'#title' => $enableLabel,
			'#attributes' => array('checked' => $enabled)
	);
	foreach ($langOpts as $langcode => $lang) {
		$form[$langcode] = array(
				'#type' => 'fieldset',
				'#title' => $lang,
				'#tree' => TRUE,
				'#collapsible' => TRUE,
				'#collapsed' => $settings['langcode'] != $langcode,
		);
	
		$form[$langcode]['settings'] = array(
				'#type' => 'fieldset',
				'#tree' => TRUE,
				'#description' => t("By default all punctuation and spaces will be removed for comparison purposes before being cast to lower case, e.g. \"Achiles' Heel\", \"Achiles Heel\", \"achilesheel\", \"achiles-heel\" and \"achiles. Heel\" will be considered the same ")
		);
		
		$form[$langcode]['settings']['langcode'] = array(
				'#type' => 'hidden',
				'#value' => $langcode
		);
		
		$form[$langcode]['settings']['removeminorwords'] = array(
				'#type' => 'checkbox',
				'#title' => 'Remove minor words',
				'#description' => 'If selected, configurable minor words will be removed for comparsion, e.g. "The Internet" will be considered the same as "Internet" alone',
		);
		
		if ($settings['removeminorwords']) {
			$form[$langcode]['settings']['removeminorwords']['#attributes'] = array('checked' => TRUE );
		}
		
		$form[$langcode]['settings']['translateaccents'] = array(
				'#type' => 'checkbox',
				'#title' => 'Translate accents',
				'#description' => 'If selected, accented characters will be replaced with plain letters for comparison, e.g. "café" will be cosidered the same as "cafe" and "Zurich" the same as "Zürich" according to language-specific rules configurable below. Please note this is for comparison purposes only and will not affect the stored term name',
		);
		if ($settings['translateaccents']) {
			$form[$langcode]['settings']['translateaccents']['#attributes'] = array('checked' => TRUE );
		}
		
		$form[$langcode]['settings']['storesynonyms'] = array(
				'#type' => 'checkbox',
				'#title' => 'Store variant term names as synonyms',
				'#description' => 'If a term name is identified as a synonym, store the typographical variant as a synonym (requires the synonym module)',
		);
		
		if ($settings['storesynonyms']) {
			$form[$langcode]['settings']['storesynonyms']['#attributes'] = array('checked' => TRUE );
		}
		
		$fuzzyMatchOpts = array(
				'none' => 'Skip',
				'endonly' => 'End of full term only',
				'wordends' => 'Word ends'
		);
		
		$form[$langcode]['settings']['fuzzymatchmode'] = array(
				'#type' => 'select',
				'#title' => 'Fuzzy matching mode',
				'#description' => t('Allow fuzzy matching of word ends (e.g. where final "s" is optional'),
				'#options' => $fuzzyMatchOpts,
				'#default_value' => $settings['fuzzymatchmode']
		);
		
		$minLengthOpts = array(3,4,5,6,7,8,9,10,11,12,13,14,15,20,25);
		$form[$langcode]['settings']['minfuzzymatchlength'] = array(
				'#type' => 'select',
				'#title' => 'Min. word or term/phrase length required for fuzzy matching',
				'#description' => t('When fuzzy matching applies only to the end of the whole term or phrase, the optimal value in English is around 10 to 15. However, when it applies to single words, select a lower number between 4 and 8.'),
				'#options' => drupal_map_assoc($minLengthOpts),
				'#default_value' => $settings['minfuzzymatchlength']
		);
		
		$form[$langcode]['settings']['fuzzymatchregex'] = array(
				'#type' => 'textfield',
				'#title' => 'Fuzzy match regular expression at ends of words',
				'#description' => t('This expression will render  a character sequence at the end of words or term phrases optional. The preprocessor will automatically add a question mark after a single character or character group in parentheses e.g. (e?s) with the words <em>wish</em> or <em>wishes</em> will both be rewritten as <em>wish(e?s)?</em> and, in theory, will match <em>wish</em>, <em>wishs</em> or <em>wishes</em>.'),
				'#default_value' => $settings['fuzzymatchregex']
		);
		$translations = array('@langName' => $defaultLangName);

		$defaultMinorWords = txorg_minor_words($langcode);
		$strWords = '';
		if (!empty($defaultMinorWords)) {
			$strWords = implode(' ',$defaultMinorWords);
		}
		$form[$langcode]['minorwords'] = array(
				'#type' => 'textarea',
				'#title' => t("Minor words"),
				'#description' => 'Enter a set of space-separated minor words to be excluded when comparing term phrases, e.g. if <em>the</em> is a minor word, <em>the Internet</em> will match <em>Internet</em>. Likewise, if <em>of</em> is another minor word <em>history of teaching</em> may be considered the same as <em>history teaching</em>, which differ semantically.',
				'#default_value' => $strWords,
				'#cols' => 60,
				'#rows' => 2
		);
		
		
		
		
		$defaultAccents = txorg_accent_map_default();
		
		$index = 0;
		$accentPairs = array();
		$numAccents = count($defaultAccents);
		if ($numAccents) {
			foreach ($defaultAccents as $source => $target) {
				$accentPairs[] = $source.'='.$target;
			}
		}
		if ($numAccents > 25) {
			$textareaRowHeight = $numAccents + 5;
		} else {
			$textareaRowHeight = 30;
		}
		$strAccentPairs = implode("\n", $accentPairs);
		$form[$langcode]['accents'] = array(
				'#type' => 'textarea',
				'#tree' => TRUE,
				'#title' => t("Accents to be simplified for comparison"),
				'#description' => t("Even if these accents are not normally used in @langName, they may appear in loanwords and proper nouns. However, there may well be some exceptions, e.g. canon is not the same as cañon or canyon",$translations),
				'#default_value' => $strAccentPairs,
				'#cols' => 8,
				'#rows' => $textareaRowHeight,
		);
		
	}
	$form['#submit'] = array('txorg_settings_form_submit');
	return system_settings_form($form);
}


function txorg_settings_form_submit($form,&$form_state) {
	$values = $form_state['values'];
	$vocabId = $values['vid'];
	$languages = language_list();
	$enabled = $values['enable'] == 1;
	variable_set('txorg_pattern_merge_' . $vocabId, $enabled);
	if (!empty($languages)) {
		foreach ($languages as $langcode => $language) {
			if ($language->enabled ==1 && isset($values[$langcode])) {
				$varKey = 'txorg_vocabulary_term_match_criteria_'.$langcode."_".$vocabId;
				$settings = $values[$langcode]['settings'];
				variable_set($varKey,$settings);
			}
		}
	}
}

function txorg_settings_form_default() {
	$default = array(
			'langcode' => 'en',
			'removeminorwords' => FALSE,
			'translateaccents' => FALSE,
			'storesynonyms' => function_exists('synonyms_extractor_info'),
			'fuzzymatchmode' => 'endonly',
			'minfuzzymatchlength' => 10,
			'fuzzymatchregex' => '(e?s)'
	);
	return $default;
}