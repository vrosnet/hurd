/* Reparent a directory

   Copyright (C) 1997 Free Software Foundation

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   The GNU Hurd is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GNU Hurd; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "priv.h"
#include "fs_S.h"

error_t
trivfs_S_dir_reparent (struct trivfs_protid *cred,
		       mach_port_t reply, mach_msg_type_name_t reply_type,
		       mach_port_t parent,
		       mach_port_t *new_dir, mach_msg_type_name_t *new_dir_type)
{
  if (cred)
    return ENOTDIR;		/* Should never be a directory */
  else
    return EOPNOTSUPP;
}
