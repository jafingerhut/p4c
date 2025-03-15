/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

#include "backends/p4tools/modules/testgen/targets/tofino/tofino2/expr_stepper.h"

#include "ir/solver.h"

#include "backends/p4tools/modules/testgen/core/extern_info.h"
#include "backends/p4tools/modules/testgen/core/program_info.h"
#include "backends/p4tools/modules/testgen/lib/execution_state.h"
#include "backends/p4tools/modules/testgen/targets/tofino/shared_expr_stepper.h"

namespace P4::P4Tools::P4Testgen::Tofino {

JBayExprStepper::JBayExprStepper(ExecutionState &state, AbstractSolver &solver,
                                 const ProgramInfo &programInfo)
    : SharedTofinoExprStepper(state, solver, programInfo) {}

// Provides implementations of JBay externs.
const JBayExprStepper::ExternMethodImpls<JBayExprStepper> JBayExprStepper::JBAY_EXTERN_METHOD_IMPLS(
    {});

void JBayExprStepper::evalExternMethodCall(const ExternInfo &externInfo) {
    auto method = JBAY_EXTERN_METHOD_IMPLS.find(externInfo.externObjectRef, externInfo.methodName,
                                                externInfo.externArguments);
    if (method.has_value()) {
        return method.value()(externInfo, *this);
    }
    // Lastly, check whether we are calling an internal extern method.
    return SharedTofinoExprStepper::evalExternMethodCall(externInfo);
}

}  // namespace P4::P4Tools::P4Testgen::Tofino
