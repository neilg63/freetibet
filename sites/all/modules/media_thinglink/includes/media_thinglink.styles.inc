<?php
// $Id:$

/**
 * @file media_thinglink/includes/media_thinglink.styles.inc
 * Styles definitions for Media: Thinglink.
 */

/**
 * Implementation of Styles module hook_styles_default_containers().
 */
function media_thinglink_styles_default_containers() {
 // We append Thinglink to the file containers.
 return array(
   'file' => array(
     'containers' => array(
       'media_thinglink' => array(
         'class' => 'MediaThinglinkStyles',
         'name' => 'media_thinglink',
         'label' => t('Thinglink'),
         'preview' => 'media_thinglink_preview_style',
       ),
     ),
   ),
 );
}

/**
 * Implementation of Styles module hook_styles_default_presets().
 */
function media_thinglink_styles_default_presets() {
  $presets = array(
    'file' => array(
      'containers' => array(
        'media_thinglink' => array(
          'default preset' => 'unlinked_thumbnail',
          'styles' => array(
            'original' => array(
              'default preset' => 'thinglink_image',
            ),
            'thumbnail' => array(
              'default preset' => 'thumb_thinglink_image',
            ),
            'square_thumbnail' => array(
              'default preset' => 'linked_square_thumbnail',
            ),
            'medium' => array(
              'default preset' => 'medium_thinglink_image',
            ),
            'large' => array(
              'default preset' => 'large_thinglink_image',
            ),
          ),
          'presets' => array(
            'thinglink_image' => array(
              array(
                'name' => 'thinglink_image',
                'settings' => array(),
              ),
            ),
            'thumb_thinglink_image' => array(
              array(
                'name' => 'resize',
                'settings' => array(
                    'width' => 100,
                ),
              ),
              array(
                'name' => 'thinglink_image',
                'settings' => array(),
              ),
            ),
            'medium_thinglink_image' => array(
              array(
                'name' => 'resize',
                'settings' => array(
                  'width' => 220,
                ),
              ),
              array(
                'name' => 'thinglink_image',
                'settings' => array(),
              ),
            ),
            'large_thinglink_image' => array(
              array(
                'name' => 'resize',
                'settings' => array(
                  'width' => 640,
                ),
              ),
              array(
                'name' => 'thinglink_image',
                'settings' => array(),
              ),
            ),
          ),
        ),
      ),
    ),
  );
   // Allow any image style to be applied to the thumbnail.
  foreach (image_styles() as $style_name => $image_style) {
    $presets['file']['containers']['media_thinglink']['presets']['linked_' . $style_name] = array(
      array(
        'name' => 'linkToMedia',
        'settings' => array(),
      ),
      array(
        'name' => 'imageStyle',
        'settings' => array(
          'image_style' => $style_name,
        ),
      ),
      array(
        'name' => 'thumbnail',
        'settings' => array(),
      ),
    );
    $presets['file']['containers']['media_thinglink']['presets']['unlinked_' . $style_name] = $presets['file']['containers']['media_thinglink']['presets']['linked_' . $style_name];
    array_shift($presets['file']['containers']['media_thinglink']['presets']['unlinked_' . $style_name]);
      foreach ($image_style['effects'] as $effect) {
      if (in_array($effect['name'], array('image_scale', 'image_scale_and_crop', 'image_resize', 'image_crop'))) {
        $presets['file']['containers']['media_thinglink']['presets']['photoset_' . $style_name] = array(
          array(
            'name' => 'resize',
            'settings' => $effect['data'],
          ),
          array(
            'name' => 'thinglink_photoset',
            'settings' => array(),
          ),
        );
      }
    }
  }
  return $presets;
}
