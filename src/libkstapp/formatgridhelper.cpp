/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 C. Barth Netterfield
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "formatgridhelper.h"
#include "viewitem.h"

#include <QDebug>

namespace Kst {

/*****************************************************************************/
/************** local helper functions for auto layout ***********************/
void appendEdge(QList<AutoFormatEdges> &edges, double pos, double size, ViewItem *item) {
  struct AutoFormatEdges edge;

  edge.edge_number = -1;
  edge.item = item;

  edge.edge = pos; // left edge
  edge.left_or_top = true;
  edges.append(edge);
  edge.edge = pos+size; // right edge
  edge.left_or_top = false;
  edges.append(edge);

}

/*****************************************************************************/
/************** local helper functions for auto layout ***********************/
bool findNextEdgeLocation(QList<AutoFormatEdges> &edges, QList<qreal> &locations, qreal grid_resolution) {
  int n_edges = edges.size();

  int i_best_edge = -1;
  int this_edge_count = 0;
  int best_edge_count = 0;

  for (int i_edge = 0; i_edge<n_edges; ++i_edge) {
    this_edge_count = 0;
    if (edges.at(i_edge).edge_number == -1) {
      // count edges that are near this edge
      for (int j_edge = 0; j_edge<n_edges; ++j_edge) {
        if (edges.at(j_edge).edge_number == -1) {
          if ((edges.at(j_edge).edge >= edges.at(i_edge).edge - 0.0001) &&
              (edges.at(j_edge).edge <= edges.at(i_edge).edge+grid_resolution)) {
            this_edge_count++;
          }
        }
      }
      if (this_edge_count > best_edge_count) {
        i_best_edge = i_edge;
        best_edge_count = this_edge_count;
      }
    }
  }
  // now i_best_edge holds the index of the edge with the most other edges
  // 'close' to it (ie, between edge and edge + grid_resolution)
  if (i_best_edge >= 0 ) {
    int edge_location = locations.size();
    qreal sum_edge = 0.0; // to get the mean of this edge
    int n_this_edge = 0;
    // Set edge_number of all of the edges which are in this area
    qreal best_edge = edges.at(i_best_edge).edge;
    for (int i_edge = 0; i_edge < n_edges; i_edge++) {
      if (edges.at(i_edge).edge_number == -1) {
        if ((edges.at(i_edge).edge >= best_edge - 0.0001) &&
            (edges.at(i_edge).edge <= best_edge+grid_resolution)) {
          edges[i_edge].edge_number = edge_location;
          sum_edge += edges.at(i_edge).edge;
          ++n_this_edge;
        }
      }
    }
    best_edge = sum_edge/qreal(n_this_edge);
    if (n_this_edge != best_edge_count) {
      qWarning() << "edge counting error in findNextEdgeLocation()";
    }
    locations.append(best_edge);
    return true;
  } else {
    return false;
  }
}

/*****************************************************************************/
/************** local helper functions for auto layout ***********************/
void convertEdgeLocationsToGrid(const QList<qreal> &locations, QList<int> &grid_locations) {

  QList<qreal> sorted_locations(locations);
  qSort(sorted_locations);

  int n_loc = locations.size();
  for (int i_unsorted = 0; i_unsorted<n_loc; i_unsorted++) {
    for (int i_sorted = 0; i_sorted<n_loc; i_sorted++) {
      if (locations.at(i_unsorted) == sorted_locations.at(i_sorted)) {
        grid_locations.append(i_sorted);
        break;
      }
    }
  }
}


/*****************************************************************************/

FormatGridHelper::FormatGridHelper(const QList<ViewItem*> &viewItems, bool protectLayout) {
  const double min_size_limit = 0.02;

  double min_height = 100000.0;
  double min_width = 100000.0;

  n_rows = 0;
  n_cols = 0;

  // Find the smallest plots, to determine the grid resolution
  int n_view = viewItems.size();
  for (int i_view = 0; i_view<n_view; i_view++) {
    ViewItem *item = viewItems.at(i_view);

    if ((item->relativeWidth()<min_width) && (item->relativeWidth()>min_size_limit)) {
      min_width = item->relativeWidth();
    }
    if ((item->relativeHeight()<min_height) && (item->relativeHeight()>min_size_limit)) {
      min_height = item->relativeHeight();
    }
  }
  double grid_x_tolerance = min_width*0.3;
  double grid_y_tolerance = min_height*0.3;

  // Find all the edges
  QList<struct AutoFormatEdges> x_edges;
  QList<struct AutoFormatEdges> y_edges;
  for (int i_view = 0; i_view<n_view; i_view++) {
    ViewItem *item = viewItems.at(i_view);

    appendEdge(x_edges, item->relativeCenter().x() - 0.5*item->relativeWidth(), item->relativeWidth(), item);
    appendEdge(y_edges, item->relativeCenter().y() - 0.5*item->relativeHeight(), item->relativeHeight(), item);
  }

  // find edge concentrations
  QList<qreal> x_edge_locations;
  QList<qreal> y_edge_locations;
  while (findNextEdgeLocation(x_edges, x_edge_locations, grid_x_tolerance)) {
  }
  while (findNextEdgeLocation(y_edges, y_edge_locations, grid_y_tolerance)) {
  }

  QList<int> x_edge_grid;
  QList<int> y_edge_grid;
  convertEdgeLocationsToGrid(x_edge_locations, x_edge_grid);
  convertEdgeLocationsToGrid(y_edge_locations, y_edge_grid);
  // x_edges: list of edges, each of which points to a x_edge_location and to a view item
  // x_edge_location: a list of where the edge concentrations are.
  // x_edge_grid: a list of grid indicies; same order as x_edge_location

  foreach (ViewItem *v, viewItems) {
    int left_gpos = 0;
    int right_gpos = 1;
    int top_gpos = 0;
    int bottom_gpos = 1;
    struct AutoFormatRC rc;
    foreach (const AutoFormatEdges &edge, x_edges) {
      if (edge.item == v) {
        if (edge.left_or_top) {
          left_gpos = x_edge_grid.at(edge.edge_number);
        } else {
          right_gpos = x_edge_grid.at(edge.edge_number);
        }
      }
    }
    foreach (const AutoFormatEdges &edge, y_edges) {
      if (edge.item == v) {
        if (edge.left_or_top) {
          top_gpos = y_edge_grid.at(edge.edge_number);
        } else {
          bottom_gpos = y_edge_grid.at(edge.edge_number);
        }
      }
    }
    rc.row = top_gpos;
    rc.col = left_gpos;
    rc.row_span = bottom_gpos - top_gpos;
    rc.col_span = right_gpos - left_gpos;
    rcList.append(rc);

    n_rows = qMax(rc.row + rc.row_span, n_rows);
    n_cols = qMax(rc.col + rc.col_span, n_cols);
  }

  a.resize(n_rows);
  for (int i_row = 0; i_row<n_rows; i_row++) {
    a[i_row].resize(n_cols);
    for (int i_col = 0; i_col<n_cols; i_col++) {
      a[i_row][i_col] = 0;
    }
  }
  QList<int> overlapedEntryIndices;
  int n_rc = rcList.length();
  for (int i_rc = 0; i_rc < n_rc; i_rc++) {
  //foreach (const struct AutoFormatRC &rc, rcList) {
    for (int i_row = rcList.at(i_rc).row; i_row<rcList.at(i_rc).row+rcList.at(i_rc).row_span; i_row++) {
      for (int i_col = rcList.at(i_rc).col; i_col<rcList.at(i_rc).col+rcList.at(i_rc).col_span; i_col++) {
        if (a[i_row][i_col]>0) {
          overlapedEntryIndices.append(i_rc);
        }
        a[i_row][i_col]++;
      }
    }
  }
  if (!protectLayout) { // remove overlaps and delete empty rows
    // move overlaps
    if (overlapedEntryIndices.length()>0) {
      foreach (const int &i_ol, overlapedEntryIndices) {
        int row, col;
        getHole(row,col);
        rcList[i_ol].row = row;
        rcList[i_ol].col = col;
      }
    }
    // remove empty rows
    condense();
  }

}

int FormatGridHelper::numHoles() {
  int n_holes = 0;
  for (int i_row = 0; i_row<n_rows; i_row++) {
    for (int i_col = 0; i_col<n_cols; i_col++) {
      if (a[i_row][i_col] == 0) {
        n_holes++;
      }
    }
  }

  return n_holes;
}

// find an unused location in the grid.  Create a new row if needed.
void FormatGridHelper::getHole(int &row, int &col) {
  for (int i_row = 0; i_row<n_rows; i_row++) {
    for (int i_col = 0; i_col<n_cols; i_col++) {
      if (a[i_row][i_col] == 0) {
        row = i_row;
        col = i_col;
        a[i_row][i_col]++; // mark it used
        return;
      }
    }
  }
  // add a new row
  n_rows++;
  a.resize(n_rows);
  a[n_rows-1].resize(n_cols);
  row = n_rows-1;
  col = 0;
  a[row][col]++; // mark it used
  return;
}

// remove empty rows and empty columns
void FormatGridHelper::condense() {
  // delete empty rows;
  for (int i_row = 0; i_row < n_rows; i_row++) {
    bool row_used = false;
    for (int i_col = 0; i_col < n_cols; i_col++) {
      if (a[i_row][i_col]>0) {
        row_used = true;
      }
    }
    if (!row_used) {
      a.remove(i_row);
      int n_rc = rcList.length();
      for (int i_rc=0; i_rc < n_rc; i_rc++) {
        if (rcList[i_rc].row > i_row) {
          rcList[i_rc].row--;
        }
      }
      i_row--;
      n_rows--;
    }
  }
  // delete empty columns
  for (int i_col = 0; i_col < n_cols; i_col++) {
    bool col_used = false;
    for (int i_row = 0; i_row < n_rows; i_row++) {
      if (a[i_row][i_col]>0) {
        col_used = true;
      }
    }
    if (!col_used) {
      for (int j_row = 0; j_row<n_rows; j_row++) {
        a[j_row].remove(i_col);
      }
      int n_rc = rcList.length();
      for (int i_rc=0; i_rc < n_rc; i_rc++) {
        if (rcList[i_rc].col > i_col) {
          rcList[i_rc].col--;
        }
      }
      i_col--;
      n_cols--;
    }
  }
}
}
