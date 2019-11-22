/*
 * JULEA - Flexible storage framework
 * Copyright (C) 2019 Benjamin Warnke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 **/

#ifndef JULEA_DB_INTERNAL_H
#define JULEA_DB_INTERNAL_H

#if !defined(JULEA_DB_H) && !defined(JULEA_DB_COMPILATION)
#error "Only <julea-db.h> can be included directly."
#endif

#include <glib.h>

#include <bson.h>

#include <julea.h>

#include <db/jdb-entry.h>
#include <db/jdb-iterator.h>
#include <db/jdb-schema.h>
#include <db/jdb-selector.h>

G_BEGIN_DECLS

struct JDBEntry
{
        JDBSchema* schema;
        bson_t bson;
        gint ref_count;
        bson_t id;
};

struct JDBIterator
{
        JDBSchema* schema;
        JDBSelector* selector;
        gpointer iterator;
        gboolean valid;
        gboolean bson_valid;
        bson_t bson;
        gint ref_count;
};

struct JDBSchemaIndex
{
        guint variable_count;
        GHashTable* variables;
};

struct JDBSchema
{
        gchar* namespace;
        gchar* name;
        gboolean bson_initialized;
        bson_t bson;
        gboolean bson_index_initialized;
        GHashTable* variables; //contains char*
        GArray* index; //contains GHashTable * which contain char*
        bson_t bson_index;
        guint bson_index_count;
        gboolean server_side;
        gint ref_count;
};

struct JDBSelector
{
        JDBSelectorMode mode;
        JDBSchema* schema;
        bson_t bson;
        guint bson_count;
        gint ref_count;
};

union JDBTypeValue
{
        guint32 val_uint32;
        gint32 val_sint32;
        guint64 val_uint64;
        gint64 val_sint64;
        gdouble val_float64;
        gfloat val_float32;
        gchar const* val_string;

        struct
        {
                gchar const* val_blob;
                guint32 val_blob_length;
        };
};

// Client-side wrappers for backend functions
gboolean j_db_internal_schema_create (JDBSchema* j_db_schema, JBatch* batch, GError** error);
gboolean j_db_internal_schema_get (JDBSchema* j_db_schema, JBatch* batch, GError** error);
gboolean j_db_internal_schema_delete (JDBSchema* j_db_schema, JBatch* batch, GError** error);
gboolean j_db_internal_insert (JDBEntry* j_db_entry, JBatch* batch, GError** error);
gboolean j_db_internal_update (JDBEntry* j_db_entry, JDBSelector* j_db_selector, JBatch* batch, GError** error);
gboolean j_db_internal_delete (JDBEntry* j_db_entry, JDBSelector* j_db_selector, JBatch* batch, GError** error);
gboolean j_db_internal_query (JDBSchema* j_db_schema, JDBSelector* j_db_selector, JDBIterator* j_db_iterator, JBatch* batch, GError** error);
gboolean j_db_internal_iterate (JDBIterator* j_db_iterator, GError** error);

gboolean j_internal_reset (gchar const* namespace, JBatch* batch, GError** error);

// Client-side additional internal functions
bson_t* j_db_selector_get_bson (JDBSelector* selector);

G_GNUC_INTERNAL JBackend* j_db_get_backend (void);

G_END_DECLS

#endif
