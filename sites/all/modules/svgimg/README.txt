#SVG Image Tag

This module lets you use SVG files in normal image fields or alternatively assign a special inline SVG formatter to a given file field where only SVG is required. Owing to limitations in Drupal's default image upload widget (that allows jpeg, png or gif only), the media module is now a dependency so you can use the improved Media File Selector widget instead. I have tested it with both Media 7.1.5 (using a bundled File Entity module) and Media 7.x-2.0-alpha4. I would recommend the latter (or more stable versions when available) if you need its media browser and full integration with File Entity 2 and use a later version of jquery. We require jquery_update with jQuery 1.7+ only for the script to replace PNGs.

Key Steps
1) In your image field settings add "svg" to the list of allowed file extensions
2) in the field widget settings select "Media File Selector" (Media 7.1x) or "Media Browser" (Media 7.2x)

Important notes:
PNG alternatives are rendered by PHP's imagemagick extension. You can check if this is available by consulting your php.ini file or simple writing a small PHP file with just phpinfo() (which will output all PHP configuration settings) and search for the words "imagick" or "ImageMagick". 
If not available, you can follow these steps to install ImageMagick:
http://php.net/manual/en/imagick.setup.php
Most static SVG file, even several MBytes, render fine. However, ImageMagick does not always render animated SVGs. These exceptions are logged under admin/reports/dblog. I can only suggest using one of many SVG Cleanup utilities if exporting from applications such as Inkscape or Illustrator.