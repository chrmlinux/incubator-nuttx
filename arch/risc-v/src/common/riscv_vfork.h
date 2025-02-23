/****************************************************************************
 * arch/risc-v/src/common/riscv_vfork.h
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

#ifndef __ARCH_RISCV_SRC_COMMON_RISCV_VFORK_H
#define __ARCH_RISCV_SRC_COMMON_RISCV_VFORK_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <arch/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Register r30 may be a frame pointer in some ABIs.  Or may just be saved
 * register s8.  It makes a difference for vfork handling.
 */

#undef VFORK_HAVE_FP

/* r0      zero   Always has the value 0.
 * r1      at     Temporary generally used by assembler.
 * r2-r3   v0-v1  Used for expression evaluations and to hold the integer and
 *                pointer type function return values.
 * r4-r7   a0-a3  Used for passing arguments to functions; values are not
 *                preserved across function calls.
 * r8-r15  t0-t7  Temporary registers used for expression evaluation; values
 *                are not preserved across function calls.
 * r16-r23 s0-s7  Saved registers; values are preserved across function
 *                calls.
 * r24-r25 t8-t9  Temporary registers used for expression evaluations; values
 *                are not preserved across function calls. When calling
 *                position independent functions r25 must contain the address
 *                of the called function.
 * r26-r27 k0-k1  Used only by the operating system.
 * r28     gp     Global pointer and context pointer.
 * r29     sp     Stack pointer.
 * r30     s8     Saved register (like s0-s7).  If a frame pointer is used,
 *                then this is the frame pointer.
 * r31     ra     Return address.
 */

#define VFORK_S0_OFFSET   (0*INT_REG_SIZE)   /* Saved register s0 */
#define VFORK_S1_OFFSET   (1*INT_REG_SIZE)   /* Saved register s1 */
#define VFORK_S2_OFFSET   (2*INT_REG_SIZE)   /* Saved register s2 */
#define VFORK_S3_OFFSET   (3*INT_REG_SIZE)   /* Saved register s3 */
#define VFORK_S4_OFFSET   (4*INT_REG_SIZE)   /* Saved register s4 */
#define VFORK_S5_OFFSET   (5*INT_REG_SIZE)   /* Saved register s5 */
#define VFORK_S6_OFFSET   (6*INT_REG_SIZE)   /* Saved register s6 */
#define VFORK_S7_OFFSET   (7*INT_REG_SIZE)   /* Saved register s7 */

#ifdef CONFIG_RISCV_FRAMEPOINTER
#  define VFORK_FP_OFFSET (8*INT_REG_SIZE)   /* Frame pointer */
#else
#  define VFORK_S8_OFFSET (8*INT_REG_SIZE)   /* Saved register s8 */
#endif

#define VFORK_SP_OFFSET   (9*INT_REG_SIZE)   /* Stack pointer*/
#define VFORK_RA_OFFSET   (10*INT_REG_SIZE)  /* Return address*/
#ifdef RISCV_SAVE_GP
#  define VFORK_GP_OFFSET (11*INT_REG_SIZE)  /* Global pointer */
#  define VFORK_SIZEOF    (12*INT_REG_SIZE)
#else
#  define VFORK_SIZEOF    (11*INT_REG_SIZE)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__
struct vfork_s
{
  /* CPU registers */

  uintptr_t s0;   /* Saved register s0 */
  uintptr_t s1;   /* Saved register s1 */
  uintptr_t s2;   /* Saved register s2 */
  uintptr_t s3;   /* Saved register s3 */
  uintptr_t s4;   /* Saved register s4 */
  uintptr_t s5;   /* Saved register s5 */
  uintptr_t s6;   /* Saved register s6 */
  uintptr_t s7;   /* Saved register s7 */
#ifdef CONFIG_RISCV_FRAMEPOINTER
  uintptr_t fp;   /* Frame pointer */
#else
  uintptr_t s8;   /* Saved register s8 */
#endif
  uintptr_t sp;   /* Stack pointer */
  uintptr_t ra;   /* Return address */
#ifdef RISCV_SAVE_GP
  uintptr_t gp;   /* Global pointer */
#endif

  /* Floating point registers (not yet) */
};
#endif

#endif /* __ARCH_RISCV_SRC_COMMON_RISCV_VFORK_H */
