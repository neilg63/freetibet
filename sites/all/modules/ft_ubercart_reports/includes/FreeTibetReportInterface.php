<?php
/**
 * @file
 */

/**
 * Reports should implement this interface.
 */
interface FreeTibetReportInterface {
  /**
   * Provides caller with the display name of the report, need not be unique but probably is.
   * @return mixed
   */
  public function getName();

  /**
   * Provides caller with the computer name of the report (should be unique - behaviour undefined otherwise).
   * @return mixed
   */
  public function getComputerName();

  /**
   * Get the filename for the output CSV.
   *
   * @return string A filename for the generated file (if one is to be generated)
   */
  public function getFilename();

  /**
   * Should produce the CSV file for the report.
   * @return mixed
   */
  public function outputCSV();

  /**
   * Sets the start and end date for a report given a year and a month.
   * @param int $month
   * @param int $year
   *
   * @return null
   */
  public function setDates($month, $year);

}
