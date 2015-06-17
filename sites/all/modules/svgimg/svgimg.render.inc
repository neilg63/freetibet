<?php

/*
* Render an SVG image as a PNG for browsers that do not support SVG
* This requires ImageMagick
* @param string $png_ref
*/
function svgimg_render($png_ref) {
	$directory_path = variable_get('file_public_path','sites/default/files');
	$path = svgimg_path();
	$current_path = $_GET['q'];
	
	$png_path = preg_replace('#'.$path.'/#', '', $current_path);
	if (preg_match('#%\d+#',$png_path)) {
		$decoded_png_path = urldecode($png_path);
	}
	else {
		$decoded_png_path = $png_path;
	}
	$full_path = DRUPAL_ROOT. DIRECTORY_SEPARATOR . $path .DIRECTORY_SEPARATOR . str_replace('/', DIRECTORY_SEPARATOR,$decoded_png_path);
	if (!file_exists($full_path)) {
		$style_parts = explode('/img-styles/', $png_path);
		$num_parts = count($style_parts);
		$svg_uri = array_pop($style_parts);
		$svg_uri = preg_replace('#\.png$#','',$svg_uri);
		$style_name = NULL;
		if ($num_parts > 1) {
			$style_name = array_shift($style_parts);
		}
		// Check if ImageMagick extension is installed
		if (class_exists('Imagick') ) {
			//$im = new Imagick();
			$svg_path = DRUPAL_ROOT. DIRECTORY_SEPARATOR . $directory_path . DIRECTORY_SEPARATOR . str_replace('/', DIRECTORY_SEPARATOR,$svg_uri);
				
			if (file_exists($svg_path)) {
				module_load_include('inc', 'svgimg');

				$svg = file_get_contents($svg_path);
				$xml = simplexml_load_string($svg);
				
				$xml->registerXPathNamespace('svg', 'http://www.w3.org/2000/svg');
				$xml->registerXPathNamespace('xlink', 'http://www.w3.org/1999/xlink');
										
				$attrs = $xml->attributes();
				$data = svgimg_fetch_style_size($style_name);

				
				if ( isset($attrs['height'])) {
					if (!empty($style_name)) {
						$data = svgimg_fetch_style_size($style_name);
					}
					$size['height'] = (int) (string) $attrs['height'];
					$size['width'] = (int) (string) $attrs['width'];
					if (!empty($style_name) && isset($data['effect'])) {
						svgimg_map_target_size($data,$size,true);
					}
				}
				else {
					if (!empty($style_name)) {
						$data = svgimg_fetch_style_size($style_name);
						$size = array(
							'width' => $data['width'],
							'height' => $data['height']
						);
					}
					else {
						$size = svgimg_default_size();
					}
				}
				$im->readImageBlob($svg);
				$im->setImageFormat("png24");
				$im->resizeImage($size['width'], $size['height'], imagick::FILTER_LANCZOS, 1);
				$parts = explode(DIRECTORY_SEPARATOR, $full_path);
				array_pop($parts);
				$full_dir = implode(DIRECTORY_SEPARATOR,$parts);
				if (!is_dir($full_dir)) {
					drupal_mkdir($full_dir,NULL,TRUE);
				}
				try {
					$im->writeImage($full_path);
				} catch (Exception $e) {
					watchdog("error", "PNG version of SVG file could not be written: " . $e->getMessage());
				}
				drupal_add_http_header('Content-Type', 'image/png');
				print $im;
				exit;
			}
		}
		else {
			watchdog("error", t("ImageMagick / Imagick is not available to convert SVG files to PNG. Please consult README.txt file for more details."));
		}
	}
	else if ($png_path != $decoded_png_path) {
		drupal_add_http_header('Content-Type', 'image/png');
		print file_get_contents($full_path);
		exit;
	}
}

/*
* Fetch assoc array of default PNG alternative width x height if none available
* @return array
*/
function svgimg_default_size() {
	$parts = explode('x', SVGIMG_DEFAULT_SIZE);
	$svgimg_default_size = array(
		'width' => (int) $parts[0],
		'height' => (int) $parts[1]
	);
	return variable_get('svgimg_default_size', $svgimg_default_size);
}
