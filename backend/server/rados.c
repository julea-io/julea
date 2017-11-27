/*
 * JULEA - Flexible storage framework
 * Copyright (C) 2010-2017 Michael Kuhn
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

#include <julea-config.h>

#include <glib.h>
#include <gmodule.h>

#include <rados/librados.h>

#include <julea.h>

/* Initialize cluster and io variables */
static rados_t backend_connection = NULL;
static rados_ioctx_t backend_io = NULL;

static gchar* backend_host = NULL;
static gchar* backend_namespace = NULL;
static gchar* backend_config = NULL;

static
gboolean
backend_create (gchar const* namespace, gchar const* path, gpointer* data)
{
	gchar* full_path = path;
	gint ret = 0;

	j_trace_file_begin(full_path, J_TRACE_FILE_CREATE);
    ret = rados_write_full(backend_io, full_path, "", 0);
	j_trace_file_end(full_path, J_TRACE_FILE_CREATE, 0, 0);

	g_free(full_path);

    return (ret == 0 ? TRUE : FALSE);
}

static
gboolean
backend_open (gchar const* namespace, gchar const* path, gpointer* data)
{
	gchar* full_path;

	full_path = g_build_filename(namespace, path, NULL);

	j_trace_file_begin(full_path, J_TRACE_FILE_OPEN);
	j_trace_file_end(full_path, J_TRACE_FILE_OPEN, 0, 0);

	*data = full_path;

	return TRUE;
}

static
gboolean
backend_delete (gpointer data)
{
	gchar* full_path = data;
	gint ret = 0;

	j_trace_file_begin(full_path, J_TRACE_FILE_DELETE);
    ret = rados_remove(backend_io, full_path);
	j_trace_file_end(full_path, J_TRACE_FILE_DELETE, 0, 0);

	g_free(full_path);

    return (ret == 0 ? TRUE : FALSE);
}

static
gboolean
backend_close (gpointer data)
{
	gchar* full_path = data;

	j_trace_file_begin(full_path, J_TRACE_FILE_CLOSE);
	j_trace_file_end(full_path, J_TRACE_FILE_CLOSE, 0, 0);

	g_free(full_path);

	return TRUE;
}

static
gboolean
backend_status (gpointer data, gint64* modification_time, guint64* size)
{
	gchar const* full_path = data;

	j_trace_file_begin(full_path, J_TRACE_FILE_STATUS);
	j_trace_file_end(full_path, J_TRACE_FILE_STATUS, 0, 0);

	*modification_time = 0;
	*size = 0;

	return TRUE;
}

static
gboolean
backend_sync (gpointer data)
{
	gchar const* full_path = data;

	j_trace_file_begin(full_path, J_TRACE_FILE_SYNC);
	j_trace_file_end(full_path, J_TRACE_FILE_SYNC, 0, 0);

	return TRUE;
}

static
gboolean
backend_read (gpointer data, gpointer buffer, guint64 length, guint64 offset, guint64* bytes_read)
{
	gchar const* full_path = data;

	(void)buffer;

	j_trace_file_begin(full_path, J_TRACE_FILE_READ);
	j_trace_file_end(full_path, J_TRACE_FILE_READ, length, offset);

	if (bytes_read != NULL)
	{
		*bytes_read = length;
	}

	return TRUE;
}

static
gboolean
backend_write (gpointer data, gconstpointer buffer, guint64 length, guint64 offset, guint64* bytes_written)
{
	gchar const* full_path = data;

	(void)buffer;

	j_trace_file_begin(full_path, J_TRACE_FILE_WRITE);
	j_trace_file_end(full_path, J_TRACE_FILE_WRITE, length, offset);

	if (bytes_written != NULL)
	{
		*bytes_written = length;
	}

	return TRUE;
}

static
gboolean
backend_init (gchar const* path)
{
	g_auto(GStrv) split = NULL;

	g_return_val_if_fail(path != NULL, FALSE);

	/* Path syntax: [hostname]:[namespace]:[config-path]
	   e.g.: localhost:testpool:/etc/ceph/ceph.conf */
	split = g_strsplit(path, ":", 0);

	backend_host = g_strdup(split[0]);
	backend_namespace = g_strdup(split[1]);
	backend_config = g_strdup(split[2]);

	g_return_val_if_fail(backend_host != NULL, FALSE);
	g_return_val_if_fail(backend_namespace != NULL, FALSE);
	g_return_val_if_fail(backend_config != NULL, FALSE);

	/* Create cluster handle */
	if(rados_create(&backend_connection, NULL) != 0)
	{
		g_critical("Can not create a RADOS cluster handle.");
	}

	/* Read config file */
	if(rados_conf_read_file(backend_connection, backend_config) != 0)
	{
		g_critical("Can not read RADOS config file %s.", backend_config);
	}

	/* Connect to cluster */
	if(rados_connect(backend_connection) != 0)
	{
		g_critical("Can not connect to RADOS %s.", backend_host);
	}

	/* Initialize IO and select pool */
	if(rados_ioctx_create(backend_connection, backend_namespace, &backend_io) != 0)
	{
		rados_shutdown(backend_connection);
		g_critical("Can not connect to RADOS pool %s.", backend_namespace);
	}

	return TRUE;
}

static
void
backend_fini (void)
{
    /* Close IO handler */
    rados_ioctx_destroy(backend_io);

    /* Close connection to cluster */
    rados_shutdown(backend_connection);

    /* Free memory */
	g_free(backend_host);
	g_free(backend_namespace);
	g_free(backend_config);
}

static
JBackend rados_backend = {
	.type = J_BACKEND_TYPE_OBJECT,
	.object = {
		.init = backend_init,
		.fini = backend_fini,
		.create = backend_create,
		.delete = backend_delete,
		.open = backend_open,
		.close = backend_close,
		.status = backend_status,
		.sync = backend_sync,
		.read = backend_read,
		.write = backend_write
	}
};

G_MODULE_EXPORT
JBackend*
backend_info (JBackendType type)
{
	JBackend* backend = NULL;

	if (type == J_BACKEND_TYPE_OBJECT)
	{
		backend = &rados_backend;
	}

	return backend;
}
