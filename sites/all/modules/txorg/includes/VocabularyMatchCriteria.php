<?php

class VocabularyMatchCriteria {

	protected $langcode = 'en';

	protected $separator = '-';

	protected $removeMinorWords = false;

	protected $translateAccents = false;

	protected $storeSynonyms = true;
	
	protected $fuzzyMatchMode = 'none';
	
	protected $allowFuzzyEnd = true;
	
	protected $minFuzzyMatchLength = 5;
	
	protected $fuzzyMatchRegex = '(e?s)';

	function __construct(array $options = array() ) {
		foreach ($options as $property => $value) {
			$property = strtolower(str_replace('_','',$property));
			switch ($property) {
				case 'langcode': case 'language': case 'lang':
					$this->langCode = (string) $value;
					break;
				case 'separator': case 'divider':
					$this->separator = (string) $value;
					break;
				case 'removeminorwords':
					$this->removeMinorWords = (bool) $value;
					break;
				case 'translateaccents': case 'translate':
					$this->translateAccents = (bool) $value;
					break;
				case 'storesynonyms': case 'storevariants': case 'synonyms':
					$this->storeSynonyms = (bool) $value;
					break;
				case 'fuzzymatchmode':
					$this->fuzzyMatchMode = $value;
					$this->allowFuzzyEnd = $this->fuzzyMatchMode != 'none'; 
					break;
				case 'minfuzzymatchlength':
					$this->minFuzzyMatchLength = (int) $value;
					break;
				case 'fuzzymatchregex':
					$this->fuzzyMatchRegex = $value;
					break;
			}
		}
	}
	
	function map() {
		$hash = array();
		$properties = array(
			'langcode',
			'removeMinorWords',
			'translateAccents',
			'storeSynonyms',
			'fuzzyMatchMode',
			'allowFuzzyEnd',
			'minFuzzyMatchLength',
			'fuzzyMatchRegex'
		);
		foreach ($properties as $property) {
			$hash[strtolower($property)] = $this->{$property};
		}
		return $hash;
	}

	function langcode() {
		return $this->langcode;
	}

	function separator() {
		return $this->separator;
	}

	function removeMinorWords() {
		return $this->removeMinorWords;
	}

	function translateAccents() {
		return $this->translateAccents;
	}
	
	function fuzzyMatchMode() {
		return $this->fuzzyMatchMode;
	}
	
	function minFuzzyMatchLength() {
		return $this->minFuzzyMatchLength;
	}
	
	function fuzzyMatchRegex() {
		return $this->fuzzyMatchRegex;
	}
	
	function allowFuzzyEnd($value = NULL) {
		if ($value === true || $value === false) {
			$this->allowFuzzyEnd = $value;
		}
		return $this->allowFuzzyEnd;
	}

	function storeSynonyms() {
		return $this->storeSynonyms;
	}

}