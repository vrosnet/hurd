/* 
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Michael I. Bushnell.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   The GNU Hurd is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "ports.h"
#include <spin-lock.h>
#include <assert.h>
#include <cthreads.h>

void
ports_manage_port_operations_multithread (struct port_bucket *bucket,
					  ports_demuxer_type demuxer,
					  int thread_timeout,
					  int global_timeout,
					  int wire_cthreads,
					  mach_port_t wire_threads)
{
  error_t err;
  int nreqthreads = 0;
  int totalthreads = 0;
  spin_lock_t lock = SPIN_LOCK_INITIALIZER;

  auto void thread_function (int);

  int 
  internal_demuxer (mach_msg_header_t *inp,
		    mach_msg_header_t *outp)
    {
      int spawn = 0;
      int status;
      struct port_info *pi;
      struct rpc_info link;

      spin_lock (&lock);
      assert (nreqthreads);
      nreqthreads--;
      if (nreqthreads == 0)
	spawn = 1;
      spin_unlock (&lock);
	  
      if (spawn)
	cthread_detach (cthread_fork ((cthread_fn_t) thread_function, 0));

      pi = ports_lookup_port (bucket, inp->msgh_local_port, 0);
      ports_begin_rpc (pi, &link);
      status = demuxer (inp, outp);
      ports_end_rpc (pi, &link);
      ports_port_deref (pi);

      spin_lock (&lock);
      nreqthreads++;
      spin_unlock (&lock);
	  
      return status;
    }

  void 
  thread_function (int master)
    {
      int timeout;
      
      if (wire_threads)
	thread_wire (wire_threads, hurd_thread_self (), 1);
      if (wire_cthreads)
	cthread_wire ();

      spin_lock (&lock);
      totalthreads++;
      nreqthreads++;
      if (master)
	timeout = global_timeout;
      else
	timeout = thread_timeout;
      spin_unlock (&lock);

    startover:

      do
	err = mach_msg_server_timeout (internal_demuxer, 0, bucket->portset,
				       timeout ? MACH_RCV_TIMEOUT : 0,
				       timeout);
      while (err != MACH_RCV_TIMED_OUT);
      
      if (master)
	{
	  spin_lock (&lock);
	  if (totalthreads != 1)
	    goto startover;
	  return;
	}
      else
	{
	  spin_lock (&lock);
	  nreqthreads--;
	  totalthreads--;
	  spin_unlock (&lock);
	  cthread_exit (0);
	}
    }
  
  thread_function (1);
}


	
      
	
