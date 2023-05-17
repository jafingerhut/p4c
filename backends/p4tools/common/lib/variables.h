#ifndef BACKENDS_P4TOOLS_COMMON_LIB_VARIABLES_H_
#define BACKENDS_P4TOOLS_COMMON_LIB_VARIABLES_H_

#include "ir/ir.h"
#include "lib/cstring.h"

namespace P4Tools {

/// Variables internal to P4Tools. These variables do not exist in the P4
/// program itself, but are generated and added to the environment by the P4Tools tooling. These
/// variables are also used for SMT solvers as symbolic variables.
class ToolsVariables {
 private:
    /// The prefix used for state variables.
    static const IR::PathExpression VAR_PREFIX;

 public:
    /// To represent header validity, we pretend that every header has a field that reflects the
    /// header's validity state. This is the name of that field. This is not a valid P4 identifier,
    /// so it is guaranteed to not conflict with any other field in the header.
    static const cstring VALID;

    /// @returns the variable with the given @type, @incarnation, and @name.
    ///
    /// A BUG occurs if this was previously called with the same @name and @incarnation, but with a
    /// different @type.
    static const IR::StateVariable &getStateVariable(const IR::Type *type, cstring name);

    /// @returns the symbolic variable with the given @type, @incarnation, and @name.
    ///
    /// A BUG occurs if this was previously called with the same @name and @incarnation, but with a
    /// different @type.
    static const IR::SymbolicVariable *getSymbolicVariable(const IR::Type *type, int incarnation,
                                                           cstring name);

    /// @see ToolsVariables::getSymbolicVariable.
    /// This function is used to generated variables caused by undefined behavior. This is merely a
    /// wrapper function for the creation of a new Taint IR object.
    static const IR::TaintExpression *getTaintExpression(const IR::Type *type);

    /// @returns the state variable for the validity of the given header instance. The resulting
    ///     variable will be boolean-typed.
    ///
    /// @param headerRef a header instance. This is either a Member or a PathExpression.
    static IR::StateVariable getHeaderValidity(const IR::Expression *headerRef);
};

}  // namespace P4Tools

#endif /* BACKENDS_P4TOOLS_COMMON_LIB_VARIABLES_H_ */
