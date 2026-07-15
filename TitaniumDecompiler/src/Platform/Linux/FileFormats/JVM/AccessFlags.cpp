#include "AccessFlags.h"

#include <stdio.h>

namespace TitaniumDecompiler {
std::string ParseJavaClassAccessFlag(int access) {
    std::string output = "";

    // Check if the access flag is a combination of multiple flags
    if (access & ACC_PUBLIC) output += "public";
    if (access & ACC_SUPER) output += (output.empty() ? "super" : " super");
    if (access & ACC_ABSTRACT) output += (output.empty() ? "abstract" : " abstract");
    if (access & ACC_FINAL) output += (output.empty() ? "final" : " final");
    if (access & ACC_INTERFACE) output += (output.empty() ? "interface" : " interface");
    if (access & ACC_ANNOTATION) output += (output.empty() ? "annotation" : " annotation");
    if (access & ACC_SYNTHETIC) output += (output.empty() ? "synthetic" : " synthetic");
    if (access & ACC_ENUM) output += (output.empty() ? "enum" : " enum");

    return output;
}

std::string ParseJavaFieldAccessFlag(int access) {
    std::string output = "";

    // Check if the access flag is a combination of multiple flags
    if (access & ACC_PUBLIC) output += "public";
    if (access & ACC_PRIVATE) output += (output.empty() ? "private" : " private");
    if (access & ACC_PROTECTED) output += (output.empty() ? "protected" : " protected");
    if (access & ACC_STATIC) output += (output.empty() ? "static" : " static");
    if (access & ACC_FINAL) output += (output.empty() ? "final" : " final");
    if (access & ACC_VOLATILE) output += (output.empty() ? "volatile" : " volatile");
    if (access & ACC_TRANSIENT) output += (output.empty() ? "transient" : " transient");
    if (access & ACC_SYNTHETIC) output += (output.empty() ? "synthetic" : " synthetic");
    if (access & ACC_ENUM) output += (output.empty() ? "enum" : " enum");

    return output;
}

std::string ParseJavaMethodsAccessFlag(int access) {
    std::string output = "";

    // Check if the access flag is a combination of multiple flags
    if (access & ACC_PUBLIC) output += "public";
    if (access & ACC_PRIVATE) output += (output.empty() ? "private" : " private");
    if (access & ACC_PROTECTED) output += (output.empty() ? "protected" : " protected");
    if (access & ACC_STATIC) output += (output.empty() ? "static" : " static");
    if (access & ACC_FINAL) output += (output.empty() ? "final" : " final");
    if (access & ACC_SYNCHRONIZED) output += (output.empty() ? "synchronized" : " synchronized");
    if (access & ACC_BRIDGE) output += (output.empty() ? "bridge" : " bridge");
    if (access & ACC_VARARGS) output += (output.empty() ? "varargs" : " varargs");
    if (access & ACC_NATIVE) output += (output.empty() ? "native" : " native");
    if (access & ACC_ABSTRACT) output += (output.empty() ? "abstract" : " abstract");
    if (access & ACC_STRICT) output += (output.empty() ? "strict" : " strict");
    if (access & ACC_SYNTHETIC) output += (output.empty() ? "synthetic" : " synthetic");

    return output;
}
}  // namespace TitaniumDecompiler