/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef incl_HPHP_ATTR_H_
#define incl_HPHP_ATTR_H_

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/*
 * Special properties on PHP classes, functions, and properties.
 *
 * Attr unions are stored as integers in .hhbc repositories, so incompatible
 * changes here require a schema version bump.
 *
 * TODO(#4513748): We're almost out of space in Attr---in fact, we already have
 * too many Attrs to fit in Class, which packs them into only 28 bits.  There's
 * no reason to share Attrs among unrelated objects, so we should really have
 * three different Attr types.
 */
enum Attr {
  AttrNone                 = 0,          // class | property | method  //
                                         //       |          |         //
  // Does this function return by reference?      |          |         //
  AttrReference            = (1u <<  0), //       |          |    X    //
                                         //       |          |         //
  // Class forbids dynamic properties?   //       |          |         //
  AttrForbidDynamicProps   = (1u <<  0), //   X   |          |         //
                                         //       |          |         //
  // Method visibility.  The relative ordering of these is important.  //
  // N.B. the values are overlayed with some of the no-override bits for magic
  // class methods (next), since they don't apply to classes.
  AttrPublic               = (1u <<  1), //       |    X     |    X    //
  AttrProtected            = (1u <<  2), //       |    X     |    X    //
  AttrPrivate              = (1u <<  3), //       |    X     |    X    //
                                         //       |          |         //
  // No-override bits for magic class methods.  If set, the class does not
  // define that magic function, and neither does any derived class.  Note that
  // the bit for __unset is further down due to Attr-sharing across types.
  AttrNoOverrideMagicGet   = (1u <<  1), //   X   |          |         //
  AttrNoOverrideMagicSet   = (1u <<  2), //   X   |          |         //
  AttrNoOverrideMagicIsset = (1u <<  3), //   X   |          |         //
  // N.B.: AttrEnum and AttrStatic overlap! But they can't be set on the
  // same things.
  // Is this class an enum?
  AttrEnum                 = (1u <<  4), //    X  |          |         //
  // Was this property's initial value supplied by the emitter (rather than a
  // user). System provided initial values can be modified to match the
  // property's type-hint.
  AttrSystemInitialValue   = (1u <<  5), //       |    X     |         //
  // Normally properties might contain KindOfNull values, even if their
  // type-hint doesn't allow this (because of initial values). This indicates
  // the property won't contain KindOfNull if its type-hint doesn't allow it.
  AttrNoImplicitNullable   = (1u <<  6), //       |    X     |         //
  // Was this declared static, abstract, or final?
  AttrStatic               = (1u <<  4), //       |    X     |    X    //
  AttrAbstract             = (1u <<  5), //    X  |          |    X    //
  AttrFinal                = (1u <<  6), //    X  |          |    X    //
                                         //       |          |         //
  // Is this class an interface?         //       |          |         //
  AttrInterface            = (1u <<  7), //    X  |          |         //
                                         //       |          |         //
  // Indicates that a static property has the <<__LSB>> attribute.
  // Such a property is implicitly redeclared in all derived classes.
  AttrLSB                  = (1u <<  7), //       |    X     |         //
  // Does this function support the async eager return optimization? If so,
  // instead of returning a finished Awaitable, this function may return the
  // unpacked result of the Awaitable, assuming the AsyncEagerRet ActRec flag
  // was set by the caller.
  AttrSupportsAsyncEagerReturn
                           = (1u <<  7), //       |          |    X    //
  // Is this class a trait?  On methods, or properties, this indicates that
  // the method was imported from a trait.
  AttrTrait                = (1u <<  8), //    X  |    X     |    X    //
                                         //       |          |         //
  // Indicates that this function should be ignored in backtraces.     //
  AttrNoInjection          = (1u <<  9), //       |          |    X    //
  // Indicates a class has no derived classes that have a magic __unset method.
  AttrNoOverrideMagicUnset = (1u <<  9), //   X   |          |         //
                                         //       |          |         //
  // Indicates this property's initial value satisfies its type-constraint and
  // no runtime check needs to be done.
  AttrInitialSatisfiesTC   = (1u <<  9), //       |    X     |         //
  // Indicates that the function or class is uniquely named among functions or
  // classes across the codebase.  Note that function and class names are in
  // separate namespaces, so it is possible to have a Func and Class which
  // share a name but both of which are unique.   |          |         //
  AttrUnique               = (1u << 10), //    X  |          |    X    //
                                         //       |          |         //
  // Indicates that this property is definitely not redeclaring a property in a
  // parent, or if it is, the type-hints of the two properties are equivalent
  // (and therefore requires no runtime check).
  AttrNoBadRedeclare       = (1u << 10), //       |    X     |         //
  // Indicates that a function can be used with fb_rename_function---even if
  // JitEnableRenameFunction is false --- and can be used with fb_intercept.
  // (Note: we could split this into two bits, since you can technically
  // pessimize less for fb_intercept than you need to for fb_rename_function,
  //  but we haven't done so at this point.)      |          |         //
  AttrInterceptable        = (1u << 11), //       |          |    X    //
                                         //       |          |         //
  // This class is sealed                //       |          |         //
  AttrSealed               = (1u << 11), //    X  |          |         //
  // Property starts as uninit, will throw if accessed before being explicitly
  // set.
  AttrLateInit             = (1u << 11), //       |    X     |         //
  // Traits have been flattened on this class.
  AttrNoExpandTrait        = (1u << 12), //    X  |          |         //
                                         //       |          |         //
  // Indicates that a function is a builtin that takes variadic arguments,
  // where the arguments are either by ref or optionally by ref.  It is
  // equivalent to ClassInfo's (RefVariableArguments).
  AttrVariadicByRef        = (1u << 12), //       |          |    X    //
                                         //       |          |         //
  // Only valid in WholeProgram mode.  Indicates on a class that the class is
  // not extended, or on a method that no extending class defines the method.
  AttrNoOverride           = (1u << 13), //    X  |          |    X    //
                                         //       |          |         //
  // The RxLevel attrs are used to encode the maximum level of reactivity
  // of a function. RxNonConditional indicates level conditionality.
  AttrRxLevel0             = (1u << 14), //       |          |    X    //
  AttrRxLevel1             = (1u << 15), //       |          |    X    //
  AttrRxNonConditional     = (1u << 16), //       |          |    X    //
                                         //       |          |         //
  // Indicates that the function, class or static property can be loaded
  // once and then persisted across all requests. |          |         //
  AttrPersistent           = (1u << 17), //    X  |    X     |    X    //
                                         //       |          |         //
  // Indicates that this property cannot be initialized on an ObjectData by
  // simply memcpy-ing from the initializer vector.          |         //
  AttrDeepInit             = (1u << 18), //       |    X     |         //
                                         //       |          |         //
  // Indicates that a function may need to use a VarEnv or varargs (i.e.,
  // extraArgs) at runtime.  If the debugger is enabled, all functions
  // must be treated as having this flag.
  AttrMayUseVV             = (1u << 18), //       |          |    X    //
                                         //       |          |         //
  // Set on functions to mark them as being able to be dynamically called
  AttrDynamicallyCallable  = (1u << 19), //       |          |    X    //
                                         //       |          |         //
  // Set on all builtin functions, whether PHP or C++.
  AttrBuiltin              = (1u << 20), //    X  |          |    X    //
                                         //       |          |         //
  // Set on all functions which take at least one inout parameter. Also implies
  // that the function takes no parameters by reference.
  AttrTakesInOutParams     = (1u << 21), //       |          |    X    //
                                         //       |          |         //
  // Set on properties to indicate they can't be changed after construction
  // and on classes to indicate that all that class' properties are immutable.
  AttrIsImmutable          = (1u << 21), //    X  |    X     |         //
                                         //       |          |         //
  // Set on classes to indicate that they have at least one immutable property.
  AttrHasImmutable         = (1u << 22), //    X  |          |         //
                                         //       |          |         //
  // Indicates that the frame should be ignored when searching for context
  // (e.g., array_map evalutates its callback in the context of the caller).
  AttrSkipFrame            = (1u << 22), //       |          |    X    //
                                         //       |          |         //
  // Set on base classes that do not have any reified classes that extend it.
  AttrNoReifiedInit        = (1u << 23), //    X  |          |         //
                                         //       |          |         //
  // Indicates that the function might read from the caller's frame. Only
  // allowed for builtins.
  AttrReadsCallerFrame     = (1u << 23), //       |          |    X    //
                                         //       |          |         //
  // Indicates that the function might write to the caller's frame. Only allowed
  // for builtins.
  AttrWritesCallerFrame    = (1u << 24), //       |          |    X    //
                                         //       |          |         //
  // Is this a (non-static) method that *must* have a non-null this?   //
  AttrRequiresThis         = (1u << 25), //       |          |    X    //
                                         //       |          |         //
  // Indicates that this function can be constant-folded if it is called with
  // all constant arguments.             //       |          |         //
  AttrIsFoldable           = (1u << 26), //       |          |    X    //
                                         //       |          |         //
  // Indicates that this function cannot be called with FCallBuiltin because it
  // requires an ActRec argument.        //       |          |         //
  AttrNoFCallBuiltin       = (1u << 27), //       |          |    X    //
                                         //       |          |         //
  // Does this function have a `...' parameter?   |          |         //
  AttrVariadicParam        = (1u << 28), //       |          |    X    //
                                         //       |          |         //
  // Indicates that a function will attempt to coerce parameters to the correct
  // type.  This isn't the same as casting---for example, a string can be cast
  // to an array, but cannot be coerced to an array.  If it fails, the function
  // returns either false or null, depending on the mode.  This behavior is
  // common in PHP5 builtins.            //       |          |         //
  AttrParamCoerceModeFalse = (1u << 29), //       |          |    X    //
  AttrParamCoerceModeNull  = (1u << 30), //       |          |    X    //
  // Indicates that this function wraps either a function taking inout or ref
  // parameters.                         //       |          |         //
  AttrIsInOutWrapper       = (1u << 31), //       |          |    X    //
};

constexpr Attr operator|(Attr a, Attr b) { return Attr((int)a | (int)b); }

inline Attr& operator|=(Attr& a, const Attr& b) {
  return (a = Attr((int)a | (int)b));
}

inline void attrSetter(Attr& attrs, bool set, Attr what) {
  if (set) {
    attrs |= what;
  } else {
    attrs = Attr(attrs & ~what);
  }
}

inline const char* attrToVisibilityStr(Attr attr) {
  return (attr & AttrPrivate)   ? "private"   :
         (attr & AttrProtected) ? "protected" : "public";
}

///////////////////////////////////////////////////////////////////////////////
}

#endif // incl_HPHP_ATTR_H_
