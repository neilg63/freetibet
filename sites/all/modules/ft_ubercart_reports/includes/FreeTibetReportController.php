<?php

if (!class_exists("FreeTibetReport")){
  require dirname(__FILE__) . '/FreeTibetReport.php';
}

/**
 * This controller handles providing the reports that are available. It's main job is to find reports and route requests
 * to the appropriate report. As such this controller is actually more of a router and the reports themselves handle some of
 * the actions that would be considered to be the domain of the controller (i.e. action dispatch) so this controller is quite
 * bare.
 *
 * The reports should be found in the reports directory (under the present one) and should implement the FreeTibetReportInterface
 * interface.
 */
class FreeTibetReportController {

  protected $reports = array();

  public function __construct(){
    $this->findReports();
  }

  /**
   * Finds all of the reports in the reports plugin directory (and makes sure that they are the right sort of thing).
   *
   * Reports are stored with computer name as the key, this is because we only expect the computer name to be unique.
   *
   * Another assumption is that the class name of the file is
   */
  protected function findReports(){
    $files = new DirectoryIterator(dirname(__FILE__) . '/reports');
    /** @var SplFileInfo $file */
    foreach ($files as $file){
      $filename = $file->getFilename();
      if ( $file->isDir() || $file->isDot() || pathinfo($filename, PATHINFO_EXTENSION) !== "php" || !$file->isReadable())
        continue;
      $className = str_replace(".php", "", $filename);
      if (!class_exists($className))
        require_once dirname(__FILE__) . '/reports/' . $filename;
      if (class_exists($className)){
        $class = new $className;
        $this->reports[$class->getComputerName()] = $class;
      }
    }
  }

  /**
   * Return the specific report that is requested (bare for direct interaction).
   *
   * @param string $report The computer name of the report demanded.
   * @return false|FreeTibetReport
   */
  public function getReport($report){
    if (isset($this->reports[$report])){
      return $this->reports[$report];
    }
    return FALSE;
  }

  /**
   * Returns a list of all of the reports as an array.
   *
   * @return array The list of reports in the format computersafename => Display name
   */
  public function getReportList(){
    $reports = array();
    if (!empty($this->reports)){
      foreach ($this->reports as $report){
        $displayName = $report->getName();
        $computerName = $report->getComputerName();
        $reports[$computerName] = $displayName;
      }
    }
    return $reports;
  }

}
