#ifndef DATA_SELECTION_HPP
#define DATA_SELECTION_HPP

#include "cgimap/types.hpp"
#include "cgimap/output_formatter.hpp"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * represents a selected set of data which can be written out to
 * an output_formatter and manipulated by a nice set of commands
 * suited for OSM relational data manipulations.
 */
class data_selection {
public:
  enum visibility_t { exists, deleted, non_exist };

  virtual ~data_selection();

  /******************* output functions ************************/

  /// write the nodes to an output formatter
  virtual void write_nodes(output_formatter &formatter) = 0;

  /// write the ways to an output formatter
  virtual void write_ways(output_formatter &formatter) = 0;

  /// write the relations to an output formatter
  virtual void write_relations(output_formatter &formatter) = 0;

  /// does this data selection support changesets?
  virtual void write_changesets(output_formatter &formatter,
                                const boost::posix_time::ptime &now);

  /******************* information functions *******************/

  // check if the node is visible, deleted or has never existed
  virtual visibility_t check_node_visibility(osm_nwr_id_t id) = 0;

  // check if the way is visible, deleted or has never existed
  virtual visibility_t check_way_visibility(osm_nwr_id_t id) = 0;

  // check if the relation is visible, deleted or has never existed
  virtual visibility_t check_relation_visibility(osm_nwr_id_t id) = 0;

  /******************* manipulation functions ******************/

  /// select the nodes in the vector, returning the number of nodes
  /// which are selected now which weren't selected before.
  virtual int select_nodes(const std::vector<osm_nwr_id_t> &) = 0;

  /// select the ways in the vector, returning the number of ways
  /// which are selected now which weren't selected before.
  virtual int select_ways(const std::vector<osm_nwr_id_t> &) = 0;

  /// select the relations in the vector, returning the number of
  /// relations which are selected now which weren't selected before.
  virtual int select_relations(const std::vector<osm_nwr_id_t> &) = 0;

  /// given a bounding box, select nodes within that bbox up to a limit of
  /// max_nodes
  virtual int select_nodes_from_bbox(const bbox &bounds, int max_nodes) = 0;

  /// selects the node members of any already selected relations
  virtual void select_nodes_from_relations() = 0;

  /// selects all ways that contain selected nodes
  virtual void select_ways_from_nodes() = 0;

  /// selects all ways that are members of selected relations
  virtual void select_ways_from_relations() = 0;

  /// select all relations that contain selected ways
  virtual void select_relations_from_ways() = 0;

  /// select nodes which are used in selected ways
  virtual void select_nodes_from_way_nodes() = 0;

  /// select relations which include selected nodes
  virtual void select_relations_from_nodes() = 0;

  /// select relations which include selected relations
  virtual void select_relations_from_relations() = 0;

  /// select relations which are members of selected relations
  virtual void select_relations_members_of_relations() = 0;

  /// does this data selection support changesets?
  virtual bool supports_changesets();

  /// select specified changesets, returning the number of
  /// changesets selected.
  virtual int select_changesets(const std::vector<osm_changeset_id_t> &);

  /// select the changeset discussions as well. this effectively
  /// just sets a flag - by default, discussions are not included,
  /// if this is called then discussions will be included.
  virtual void select_changeset_discussions();

  /**
   * factory for the creation of data selections. this abstracts away
   * the creation process of transactions, and allows some up-front
   * work to be done. for example, setting up prepared statements on
   * a database connection.
   */
  struct factory {
    virtual ~factory();

    /// get a handle to a selection which can be used to build up
    /// a working set of data.
    virtual boost::shared_ptr<data_selection> make_selection() = 0;
  };
};

typedef boost::shared_ptr<data_selection::factory> factory_ptr;

#endif /* DATA_SELECTION_HPP */
