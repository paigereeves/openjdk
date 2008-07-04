/*
 * Copyright 2001-2008 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

// Stub class generated by rmic, do not edit.
// Contents subject to change without notice.

public final class MarshalAfterUnexport_Stub
    extends java.rmi.server.RemoteStub
    implements Receiver, java.rmi.Remote
{
    private static final long serialVersionUID = 2;

    private static java.lang.reflect.Method $method_receive_0;

    static {
        try {
            $method_receive_0 = Receiver.class.getMethod("receive", new java.lang.Class[] {java.rmi.Remote.class});
        } catch (java.lang.NoSuchMethodException e) {
            throw new java.lang.NoSuchMethodError(
                "stub class initialization failed");
        }
    }

    // constructors
    public MarshalAfterUnexport_Stub(java.rmi.server.RemoteRef ref) {
        super(ref);
    }

    // methods from remote interfaces

    // implementation of receive(Remote)
    public void receive(java.rmi.Remote $param_Remote_1)
        throws java.rmi.RemoteException
    {
        try {
            ref.invoke(this, $method_receive_0, new java.lang.Object[] {$param_Remote_1}, 5876293363550629411L);
        } catch (java.lang.RuntimeException e) {
            throw e;
        } catch (java.rmi.RemoteException e) {
            throw e;
        } catch (java.lang.Exception e) {
            throw new java.rmi.UnexpectedException("undeclared checked exception", e);
        }
    }
}
