/*
 * Copyright (C) 2007 Michael Brown <mbrown@fensystems.co.uk>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>
#include <errno.h>
#include <gpxe/xfer.h>

/** @file
 *
 * Data transfer interfaces
 *
 */

/**
 * Close data transfer interface
 *
 * @v xfer		Data transfer interface
 * @v rc		Reason for close
 */
void xfer_close ( struct xfer_interface *xfer, int rc ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );

	dest->op->close ( dest, rc );
	xfer_unplug ( xfer );
	xfer_put ( dest );
}

/**
 * Send redirection event
 *
 * @v xfer		Data transfer interface
 * @v type		New location type
 * @v args		Remaining arguments depend upon location type
 * @ret rc		Return status code
 */
int xfer_vredirect ( struct xfer_interface *xfer, int type, va_list args ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	int rc;

	rc = dest->op->vredirect ( dest, type, args );
	xfer_put ( dest );
	return rc;
}

/**
 * Send redirection event
 *
 * @v xfer		Data transfer interface
 * @v type		New location type
 * @v ...		Remaining arguments depend upon location type
 * @ret rc		Return status code
 */
int xfer_redirect ( struct xfer_interface *xfer, int type, ... ) {
	va_list args;
	int rc;

	va_start ( args, type );
	rc = xfer_vredirect ( xfer, type, args );
	va_end ( args );
	return rc;
}

/**
 * Request data
 *
 * @v xfer		Data transfer interface
 * @v offset		Offset to new position
 * @v whence		Basis for new position
 * @v len		Length of requested data
 * @ret rc		Return status code
 */
int xfer_request ( struct xfer_interface *xfer, off_t offset, int whence,
		   size_t len ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	int rc;

	rc = dest->op->request ( dest, offset, whence, len );
	xfer_put ( dest );
	return rc;
}

/**
 * Request all data
 *
 * @v xfer		Data transfer interface
 * @ret rc		Return status code
 */
int xfer_request_all ( struct xfer_interface *xfer ) {
	return xfer_request ( xfer, 0, SEEK_SET, ~( ( size_t ) 0 ) );
}

/**
 * Seek to position
 *
 * @v xfer		Data transfer interface
 * @v offset		Offset to new position
 * @v whence		Basis for new position
 * @ret rc		Return status code
 */
int xfer_seek ( struct xfer_interface *xfer, off_t offset, int whence ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	int rc;

	rc = dest->op->seek ( dest, offset, whence );
	xfer_put ( dest );
	return rc;
}

/**
 * Allocate I/O buffer
 *
 * @v xfer		Data transfer interface
 * @v len		I/O buffer payload length
 * @ret iobuf		I/O buffer
 */
struct io_buffer * xfer_alloc_iob ( struct xfer_interface *xfer, size_t len ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	struct io_buffer *iobuf;

	iobuf = dest->op->alloc_iob ( dest, len );
	xfer_put ( dest );
	return iobuf;
}

/**
 * Deliver datagram
 *
 * @v xfer		Data transfer interface
 * @v iobuf		Datagram I/O buffer
 * @ret rc		Return status code
 */
int xfer_deliver_iob ( struct xfer_interface *xfer, struct io_buffer *iobuf ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	int rc;

	rc = dest->op->deliver_iob ( dest, iobuf );
	xfer_put ( dest );
	return rc;
}

/**
 * Deliver datagram as raw data
 *
 * @v xfer		Data transfer interface
 * @v iobuf		Datagram I/O buffer
 * @ret rc		Return status code
 */
int xfer_deliver_raw ( struct xfer_interface *xfer,
		       const void *data, size_t len ) {
	struct xfer_interface *dest = xfer_get_dest ( xfer );
	int rc;

	rc = dest->op->deliver_raw ( dest, data, len );
	xfer_put ( dest );
	return rc;
}

/****************************************************************************
 *
 * Helper methods
 *
 * These functions are designed to be used as methods in the
 * xfer_interface_operations table.
 *
 */

/**
 * Ignore close() event
 *
 * @v xfer		Data transfer interface
 * @v rc		Reason for close
 */
void ignore_xfer_close ( struct xfer_interface *xfer __unused,
			 int rc __unused ) {
	/* Nothing to do */
}

/**
 * Ignore vredirect() event
 *
 * @v xfer		Data transfer interface
 * @v type		New location type
 * @v args		Remaining arguments depend upon location type
 * @ret rc		Return status code
 */
int ignore_xfer_vredirect ( struct xfer_interface *xfer __unused,
			    int type __unused, va_list args __unused ) {
	return 0;
}

/**
 * Ignore request() event
 *
 * @v xfer		Data transfer interface
 * @v offset		Offset to new position
 * @v whence		Basis for new position
 * @v len		Length of requested data
 * @ret rc		Return status code
 */
int ignore_xfer_request ( struct xfer_interface *xfer __unused,
			  off_t offset __unused, int whence __unused, 
			  size_t len __unused ) {
	return 0;
}

/**
 * Ignore seek() event
 *
 * @v xfer		Data transfer interface
 * @v offset		Offset to new position
 * @v whence		Basis for new position
 * @ret rc		Return status code
 */
int ignore_xfer_seek ( struct xfer_interface *xfer __unused,
		       off_t offset __unused, int whence __unused ) {
	return 0;
}

/**
 * Allocate I/O buffer
 *
 * @v xfer		Data transfer interface
 * @v len		I/O buffer payload length
 * @ret iobuf		I/O buffer
 */
struct io_buffer *
default_xfer_alloc_iob ( struct xfer_interface *xfer __unused, size_t len ) {
	return alloc_iob ( len );
}

/**
 * Deliver datagram as raw data
 *
 * @v xfer		Data transfer interface
 * @v iobuf		Datagram I/O buffer
 * @ret rc		Return status code
 *
 * This function is intended to be used as the deliver() method for
 * data transfer interfaces that prefer to handle raw data.
 */
int xfer_deliver_as_raw ( struct xfer_interface *xfer,
			  struct io_buffer *iobuf ) {
	int rc;

	rc = xfer->op->deliver_raw ( xfer, iobuf->data, iob_len ( iobuf ) );
	free_iob ( iobuf );
	return rc;
}

/**
 * Deliver datagram as I/O buffer
 *
 * @v xfer		Data transfer interface
 * @v data		Data buffer
 * @v len		Length of data buffer
 * @ret rc		Return status code
 *
 * This function is intended to be used as the deliver_raw() method
 * for data transfer interfaces that prefer to handle I/O buffers.
 */
int xfer_deliver_as_iob ( struct xfer_interface *xfer,
			  const void *data, size_t len ) {
	struct io_buffer *iobuf;

	iobuf = xfer->op->alloc_iob ( xfer, len );
	if ( ! iobuf )
		return -ENOMEM;

	memcpy ( iob_put ( iobuf, len ), data, len );
	return xfer->op->deliver_iob ( xfer, iobuf );
}

/**
 * Ignore datagram as raw data event
 *
 * @v xfer		Data transfer interface
 * @v data		Data buffer
 * @v len		Length of data buffer
 * @ret rc		Return status code
 */
int ignore_xfer_deliver_raw ( struct xfer_interface *xfer,
			      const void *data __unused, size_t len ) {
	DBGC ( xfer, "XFER %p %zd bytes delivered %s\n", xfer, len,
	       ( ( xfer == &null_xfer ) ?
		 "before connection" : "after termination" ) );
	return 0;
}

/** Null data transfer interface operations */
struct xfer_interface_operations null_xfer_ops = {
	.close		= ignore_xfer_close,
	.vredirect	= ignore_xfer_vredirect,
	.request	= ignore_xfer_request,
	.seek		= ignore_xfer_seek,
	.alloc_iob	= default_xfer_alloc_iob,
	.deliver_iob	= xfer_deliver_as_raw,
	.deliver_raw	= ignore_xfer_deliver_raw,
};

/**
 * Null data transfer interface
 *
 * This is the interface to which data transfer interfaces are
 * connected when unplugged.  It will never generate messages, and
 * will silently absorb all received messages.
 */
struct xfer_interface null_xfer = {
	.intf = {
		.dest = &null_xfer.intf,
		.refcnt = NULL,
	},
	.op = &null_xfer_ops,
};
