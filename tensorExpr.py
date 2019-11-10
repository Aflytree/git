#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#================================================================
#   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
#   
#   @name          tensorExpr.py
#   @author        ：Afly
#   @date          ：2019.08.24
#   @description   ：
#
#================================================================
from __future__ import absolute_import, print_function
import numpy as np
import tvm

tgt_host="llvm"
tgt="cuda"

n=tvm.var("n")
A=tvm.placeholder((n,), name='A')
B=tvm.placeholder((n,), name='B')
print("A.shape:")
print(A.shape)

C = tvm.compute(A.shape, lambda i: A[i]+B[i], name="C")
#print(type(C))

s = tvm.create_schedule(C.op)

bx, tx = s[C].split(C.op.axis[0], factor=64)

if tgt=="cuda" or tgt.startswith('opencl'):
    s[C].bind(bx, tvm.thread_axis("blockIdx.x"))
    s[C].bind(tx, tvm.thread_axis("blockIdx.x"))

fadd = tvm.build(s, [A, B, C], tgt, target_host=tgt_host, name="myadd")
ctx = tvm.context(tgt, 0)

n = 2
a = tvm.nd.array(np.random.uniform(size=n).astype(A.dtype), ctx)
b = tvm.nd.array(np.random.uniform(size=n).astype(B.dtype), ctx)
print("a:")
print(a)
print("b:")
print(b)
c = tvm.nd.array(np.zeros(n, dtype=C.dtype), ctx)
fadd(a, b, c)
print("fadd c:")
print(c)
print("c.asnumpy():")
print(c.asnumpy())
print("numpy result:")
print(a.asnumpy()+b.asnumpy())
tvm.testing.assert_allclose(c.asnumpy(), a.asnumpy() + b.asnumpy())


if tgt == "cuda" or tgt.startswith('opencl'):
    dev_module = fadd.imported_modules[0]
    print("-----GPU code-----")
    print(dev_module.get_source())
else:
    print(fadd.get_source())
