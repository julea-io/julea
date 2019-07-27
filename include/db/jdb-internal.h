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

#ifndef JULEA_DB_INTERNAL_H
#define JULEA_DB_INTERNAL_H

#include <glib.h>
#include <bson.h>
#include <julea.h>
#include <db/jdb-selector.h>

//client side wrappers for backend functions
gboolean j_db_internal_schema_create(gchar const* namespace, gchar const* name, bson_t const* schema, JBatch* batch, GError** error);
gboolean j_db_internal_schema_get(gchar const* namespace, gchar const* name, bson_t* schema, JBatch* batch, GError** error);
gboolean j_db_internal_schema_delete(gchar const* namespace, gchar const* name, JBatch* batch, GError** error);
gboolean j_db_internal_insert(gchar const* namespace, gchar const* name, bson_t const* metadata, JBatch* batch, GError** error);
gboolean j_db_internal_update(gchar const* namespace, gchar const* name, bson_t const* selector, bson_t const* metadata, JBatch* batch, GError** error);
gboolean j_db_internal_delete(gchar const* namespace, gchar const* name, bson_t const* selector, JBatch* batch, GError** error);
gboolean j_db_internal_query(gchar const* namespace, gchar const* name, bson_t const* selector, gpointer* iterator, JBatch* batch, GError** error);
gboolean j_db_internal_iterate(gpointer iterator, bson_t* metadata, GError** error);

#endif
