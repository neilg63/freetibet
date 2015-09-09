<?php
/**
 * @file
 * Returns the HTML for a single Drupal page.
 *
 * Complete documentation for this file is available online.
 * @see https://drupal.org/node/1728148
 */
?>
<div id="page-wrapper">

  <header class="header" id="header" role="banner">

    <?php if ($logo): ?>
      <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="header__logo" id="logo"><span class="alt-text"><?php print t('Home'); ?></span></a>
    <?php endif; ?>
		<?php print render($page['top']); ?>

        <?php if ($site_slogan): ?>
          <div class="header__site-slogan" id="site-slogan"><?php print $site_slogan; ?></div>
        <?php endif; ?>
    
		<?php print render($page['header']); ?>
  </header>

  <div id="main">
    <div class="header-rule"></div>
		
    <section id="content" class="column" role="main">
      <?php print $breadcrumb; ?>
      <a id="main-content"></a>
      <?php if ($title): ?>
        <h1 class="page__title title" id="page-title"><?php print $title; ?></h1>
      <?php endif; ?>
      <?php print render($page['highlighted']); ?>
			<?php print render($tabs); ?>
      <?php print $messages; ?>
      <?php print render($page['help']); ?>
      <?php if ($action_links): ?>
        <ul class="action-links"><?php print render($action_links); ?></ul>
      <?php endif; ?>
      <?php print render($page['content']); ?>
      <?php print $feed_icons; ?>
    </section>
    <?php if ($has_aside): ?>
      <aside class="sidebar">
        <?php print $aside; ?>
      </aside>
    <?php endif; ?>
    <?php print render($page['content_bottom']); ?>
  </div>

  <?php print render($page['footer']); ?>

</div>

<?php print render($page['bottom']); ?>
