<?php

/*
* Render an SVG image as a PNG for browsers that do not support SVG
* This requires ImageMagick
* @param string $png_ref
*/
function svgimg_render($png_ref) {
  $directory_path = variable_get('file_public_path', conf_path() . '/files');
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
    
    $im_path = variable_get('svgimg_imagemagick',NULL);
    // Check if ImageMagick extension is installed
    $valid = class_exists('Imagick');
    $use_php = $valid;
    if (!$valid) {
      if ($im_path != 'php' && is_string($im_path) && strlen($im_path) > 3) {
        $use_php = false;
        $valid = true;
      }
    }
    if ($valid) {
      $parts = explode(DIRECTORY_SEPARATOR, $full_path);
      array_pop($parts);
      $full_dir = implode(DIRECTORY_SEPARATOR,$parts);
      if (!is_dir($full_dir)) {
        drupal_mkdir($full_dir,NULL,TRUE);
      }
      if (strpos($png_path,'svgimg/samples/') === 0) {
        $source_directory_path = str_replace('/svgimg','', drupal_get_path('module', 'svgimg'));
      }
      else {
        $source_directory_path = $directory_path;
      }
       module_load_include('inc', 'svgimg');
      $svg_path = svgimg_root_path($svg_uri,$source_directory_path);
      if (file_exists($svg_path)) {
        $svg_data = svgimg_fetch_data($svg_path);
        $size = $svg_data->size();
        $resized = FALSE;
        if (!empty($style_name)) {
          $data = svgimg_fetch_style_size($style_name);
        }
        $source_width = $size['width'];
        if (!empty($style_name) && isset($data['effect'])) {
          svgimg_map_target_size($data,$size,true);
        }
        else {
          if (!empty($style_name)) {
            $data = svgimg_fetch_style_size($style_name);
            $size = array(
              'width' => $data['width'],
              'height' => $data['height']
            );
          }
          else if ($svg_data->hasSize() == false) {
            $size = svgimg_default_size();
          }
        }
        // Set SVG root tag to have target width and height parameters defined by 
        // image style to prevent pixelation on resize as ImageMagick rasterises
        // SVG in readImageBlock method
        if (!empty($style_name)) {
          $resized = $source_width != $size['width'];
          if ($resized) {
            $svg_data->setSize($size);
            $size_reset = true;
          }
        }
        $svg = $svg_data->svg();
        $valid_svg = $svg_data->valid();
        if ($valid_svg) {
          if ($use_php) {
            _svgimg_php_render($full_path,$svg,$size);
          }
          else {
            _svgimg_shell_render($im_path,$full_path,$svg_path,$size);
          }
        }
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
* @param string $full_path
* @param string $svg
* @param array $size
*/
function _svgimg_php_render($full_path = '', $svg = '', array $size = array()) {
  $im = new Imagick();
  try {
    $im->readImageBlob($svg);
  }
  catch (Exception $e) {
    watchdog("error", "SVG file could not be parsed for PNG conversion: " . $e->getMessage());
    $valid_svg = false;
  }
  if ($valid_svg) {
    $im->setImageFormat("png24");
    $im->resizeImage($size['width'], $size['height'], imagick::FILTER_LANCZOS, 1);
    
    try {
      $im->writeImage($full_path);
    }
    catch (Exception $e) {
      watchdog("error", "PNG version of SVG file could not be written: " . $e->getMessage());
    }
    svgimg_print_stream($im);
    exit;
  }
}

/*
* @param string $im_path
* @param string $full_path
* @param string $svg_path
* @param array $size
*/
function _svgimg_shell_render($im_path = NULL,$full_path,$svg_path,array $size = array()) {
  $command = $im_path . ' -background none -size '.$size['width'] .'x'.$size['height'].' '.$svg_path.' ' . $full_path;
  exec($command);
  usleep(100000);
  if (file_exists($full_path)) {
    $stream = file_get_contents($full_path);
    svgimg_print_stream($stream);
  }
}

function svgimg_print_stream($stream = NULL) {
  drupal_add_http_header('Content-Type', 'image/png');
  drupal_add_http_header('Expires','0');
  if (is_string($stream)) {
    drupal_add_http_header('Content-Length', strlen($stream));
  }
  print $stream;
  exit;
}