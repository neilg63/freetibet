<?php
$im = new Imagick();
$fn = 'Aegean_with_legends.svg';
$nfn = preg_replace('#\.svg$#','.png',$fn);
if (!file_exists($nfn) || 1==1) {


$svg = file_get_contents($fn);

$xml = simplexml_load_string($svg); 

$xml->registerXPathNamespace('svg', 'http://www.w3.org/2000/svg');
$xml->registerXPathNamespace('xlink', 'http://www.w3.org/1999/xlink');

$attrs = $xml->attributes();
if ( isset($attrs['height'])) {
	$height = (int) (string) $attrs['height'];
	$width = (int) (string) $attrs['width'];

	$im->readImageBlob($svg);

	$im->setImageFormat("png24");
	$im->resizeImage($width, $height, imagick::FILTER_LANCZOS, 1);
}

$im->writeImage( $nfn  );/*(or .jpg)*/
$im->clear();
$im->destroy();
}

print '<img src="'.$nfn.'" />';
print '<img src="'.$fn.'" />';
