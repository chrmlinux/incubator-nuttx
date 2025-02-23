/****************************************************************************
 * arch/x86/src/common/up_exit.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sched.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#ifdef CONFIG_DUMP_ON_EXIT
#  include <nuttx/fs/fs.h>
#endif

#include "task/task.h"
#include "sched/sched.h"
#include "group/group.h"
#include "up_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_DEBUG_SCHED_INFO
#  undef CONFIG_DUMP_ON_EXIT
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: _up_dumponexit
 *
 * Description:
 *   Dump the state of all tasks whenever on task exits.  This is debug
 *   instrumentation that was added to check file-related reference counting
 *   but could be useful again sometime in the future.
 *
 ****************************************************************************/

#ifdef CONFIG_DUMP_ON_EXIT
static void _up_dumponexit(FAR struct tcb_s *tcb, FAR void *arg)
{
  FAR struct filelist *filelist;
  int i;
  int j;

  sinfo("  TCB=%p name=%s pid=%d\n", tcb, tcb->name, tcb->pid);
  sinfo("    priority=%d state=%d\n", tcb->sched_priority, tcb->task_state);

  filelist = &tcb->group->tg_filelist;
  for (i = 0; i < filelist->fl_rows; i++)
    {
      for (j = 0; j < CONFIG_NFILE_DESCRIPTORS_PER_BLOCK; j++)
        {
          struct inode *inode = filelist->fl_files[i][j].f_inode;
          if (inode)
            {
              sinfo("      fd=%d refcount=%d\n",
                    i * CONFIG_NFILE_DESCRIPTORS_PER_BLOCK + j,
                    inode->i_crefs);
            }
        }
    }
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_exit
 *
 * Description:
 *   This function causes the currently executing task to cease to exist.
 *   This is a special case of task_delete() where the task to be deleted is
 *   the currently executing task.  It is more complex because a context
 *   switch must be perform to the next ready to run task.
 *
 ****************************************************************************/

void up_exit(int status)
{
  struct tcb_s *tcb = this_task();

  /* Make sure that we are in a critical section with local interrupts.
   * The IRQ state will be restored when the next task is started.
   */

  enter_critical_section();

  sinfo("TCB=%p exiting\n", tcb);

  /* Destroy the task at the head of the ready to run list. */

  nxtask_exit();

#ifdef CONFIG_DUMP_ON_EXIT
  sinfo("Other tasks:\n");
  nxsched_foreach(_up_dumponexit, NULL);
#endif

  /* Now, perform the context switch to the new ready-to-run task at the
   * head of the list.
   */

  tcb = this_task();

  /* Adjusts time slice for SCHED_RR & SCHED_SPORADIC cases
   * NOTE: the API also adjusts the global IRQ control for SMP
   */

  nxsched_resume_scheduler(tcb);

#ifdef CONFIG_ARCH_ADDRENV
  /* Make sure that the address environment for the previously running
   * task is closed down gracefully (data caches dump, MMU flushed) and
   * set up the address environment for the new thread at the head of
   * the ready-to-run list.
   */

  group_addrenv(tcb);
#endif

  /* Then switch contexts */

  up_fullcontextrestore(tcb->xcp.regs);
}
