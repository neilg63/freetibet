<?php


function svgimg_render($png_ref) {
	$directory_path = variable_get('file_public_path','sites/default/files');
	$path = svgimg_path();
	$current_path = $_GET['q'];
	
	$png_path = preg_replace('#'.$path.'/#', '', $current_path);
	
	$full_path = DRUPAL_ROOT. DIRECTORY_SEPARATOR . $path .DIRECTORY_SEPARATOR . str_replace('/', DIRECTORY_SEPARATOR,$png_path);
	if (!file_exists($full_path)) {
		$style_parts = explode('/img-styles/', $png_path);
		$num_parts = count($style_parts);
		$svg_uri = array_pop($style_parts);
		$svg_uri = preg_replace('#\.png$#','',$svg_uri);
		$style_name = NULL;
		if ($num_parts > 1) {
			$style_name = array_shift($style_parts);
		}
		if (class_exists('Imagick')) {
			$im = new Imagick();
			$svg_path = DRUPAL_ROOT. DIRECTORY_SEPARATOR . $directory_path . DIRECTORY_SEPARATOR . str_replace('/', DIRECTORY_SEPARATOR,$svg_uri);
				
			if (file_exists($svg_path)) {
				$svg = file_get_contents($svg_path);
				$xml = simplexml_load_string($svg); 

				$xml->registerXPathNamespace('svg', 'http://www.w3.org/2000/svg');
				$xml->registerXPathNamespace('xlink', 'http://www.w3.org/1999/xlink');
										
				$attrs = $xml->attributes();
				if ( isset($attrs['height'])) {
					if (!empty($style_name)) {
						$data = svgimg_fetch_style_size($style_name);
					}
							
					$size['height'] = (int) (string) $attrs['height'];
					$size['width'] = (int) (string) $attrs['width'];
					if (!empty($style_name) && isset($data['effect'])) {
						svgimg_map_target_size($data,$size,true);
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
	}
	
	
	}
	

}