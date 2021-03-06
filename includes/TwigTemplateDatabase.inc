<?php

/**
 * @file
 * Provides a wrapper class around the database table were Twig Templates are stored.
 */

class TwigTemplateDatabase {

  /**
   * Constants
   */
  const TABLE = 'islandora_multi_importer_twig_templates';

  // @deprecated Constants
  // @codingStandardsIgnoreStart
  const table = self::TABLE;
  // @codingStandardsIgnoreEnd

  /**
   * Checks to see if the given Twig Template exists in the database.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   *
   * @return bool
   *   TRUE if the given Twig Template exists, FALSE otherwise.
   */
  public static function Exists($twig_name) {
    $count = db_select('islandora_multi_importer_twig_templates', 'tw')
      ->condition('tw.name', $twig_name)
      ->fields('tw', array('name'))
      ->execute()
      ->rowCount();

    return $count == 1;
  }


  /**
   * Checks to see if the given Twig Template exists in the database.
   *
   * @param int $twig_id
   *   The unique id of the Twig Template.
   *
   * @return bool
   *   TRUE if the given Twig Template exists, FALSE otherwise.
   */
  public static function ExistsbyId($twig_id) {
    $count = db_select('islandora_multi_importer_twig_templates', 'tw')
      ->condition('tw.id', $twig_id)
      ->fields('tw', array('name'))
      ->execute()
      ->rowCount();

    return $count == 1;
  }

  /**
   * Checks to see if the given Twig Template exists in the database and is valid.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   *
   * @return bool
   *   TRUE if the given Twig Template exists, FALSE otherwise.
   */
  public static function Valid($twig_name) {
    if (self::Exists($twig_name)) {
      return self::Get($twig_name) !== FALSE;
    }
    return FALSE;
  }

  /**
   * Gets the Twig Template identified by name.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   *
   * @return DOMDocument
   *   The Twig Template if found, FALSE otherwise.
   */
  public static function Get($twig_name) {
    if (self::Exists($twig_name)) {
      $query = db_query('SELECT twig FROM {islandora_multi_importer_twig_templates} WHERE name = :name', array(':name' => $twig_name));
      $twig = $query->fetchField();
      if (trim($twig) !== '') {
        return  $twig;
      }
    }
    return FALSE;
  }
  
  /**
   * Gets the Twig Template identified by id.
   *
   * @param int $id
   *   The name of the Twig Template.
   *
   * @return Array
   *   A full record with all the info
   */
  public static function GetbyId($id) {
      $query = db_query('SELECT twig, name, updated FROM {islandora_multi_importer_twig_templates} WHERE id = :id', array(':id' => $id));
      $twiginfo = $query->fetchAssoc('id');
      return $twiginfo;
  }



  /**
   * Gets a list of all defined Twig Template names.
   *
   * @return array
   *   An array of defined Twig Template names, where both the key and the value are the
   *   Twig Template's name; e.g. array('name' => 'name').
   */
  public static function GetNames() {
    $names = array();
    $result = db_query('SELECT name FROM {islandora_multi_importer_twig_templates}')->fetchCol();
    foreach ($result as $data) {
      $names[$data] = $data;
    }
    return $names;
  }
  
  /**
   * Gets a list of all defined Twig Template names.
   *
   * @return array
   *   An array of defined Twig Template names, where key is the unique id of 
   *   the Twig Template's and value associative array of 
   *   'name' => thename and 'updated' => date updated
   */
  public static function GetProperties() {
    $names = array();
    $result = db_query('SELECT id, name,updated FROM {islandora_multi_importer_twig_templates}');
  
    while ($data = $result->fetchAssoc('id')) {
      $names[$data['id']] = array('name' => $data['name'], 'updated' => $data['updated']);
    }
    return $names;
  }


  /**
   * Creates a Twig Template with the given Twig Template name and definition.
   *
   * If the Twig Template already exists it will fail.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   * @param string $twigTemplate
   *   An Twig Template.
   *
   * @return bool
   *   TRUE if successful, otherwise FALSE.
   */
  public static function Create($twig_name, $twigTemplate = NULL) {
    if (!self::Exists($twig_name)) {
      $object = new stdClass();
      $object->name = $twig_name;
      $object->twig = trim($twigTemplate);
      $object->updated = time();
      return drupal_write_record(self::TABLE, $object) !== FALSE;
    }
    return FALSE;
  }

  /**
   * Copies an existing Twig Template.
   *
   * @param string $twig_name_src
   *   The name of the source Twig Template to copy from.
   * @param string $twig_name_dest
   *   The name of the destination Twig Template which gets copied to.
   *
   * @return bool
   *   TRUE if successful; FALSE otherwise.
   */
  public static function Copy($twig_name_src, $twig_name_dest) {
    if (self::Exists($twig_name_src)) {
      $template = self::Get($twig_name_src);
      return self::Create($twig_name_dest, $template);
    }
    return FALSE;
  }

  
  /**
   * Updates the Twig Template with the given Twig Template name; the previous definition is lost.
   *
   * If the Twig Template does not exist, this function will fail.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   * @param string $twigTemplaten
   *   A Twig Template.
   *
   * @returns bool
   *   TRUE if successful; FALSE otherwise.
   */
  public static function UpdatebyId($twig_id, $twig_name, $twigTemplate) {
    if (self::ExistsById($twig_id)) {
     
      $num = db_update(self::TABLE)
        ->fields(array('twig' => $twigTemplate, 'updated' => time(),'name' => trim($twig_name)))
        ->condition('id', $twig_id)
        ->execute();
      if ($num) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
    return FALSE;
  }

  /**
   * Deletes the Twig Template with the given Twig Template name.
   *
   * @param string $twig_name
   *   The name of the Twig Template.
   *
   * @return bool
   *   TRUE if successful; FALSE otherwise.
   */
  public static function DeleteById($twig_id) {
    if (self::ExistsById($twig_id)) {
      $num = db_delete(self::TABLE)
        ->condition('id', $twig_id)
        ->execute();
      if ($num) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
    return FALSE;
  }

}
