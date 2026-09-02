Classfile /home/austin/Github/Decompiler-Kotlin/JavaTestFiles/TestNewArray.class
  Last modified Feb 16, 2025; size 884 bytes
  SHA-256 checksum 8d4f54a97851cdfad4efbdd52a4d2aacfe59f05efa8f28373a8c91304d0ff9c0
  Compiled from "TestNewArray.java"
public class JavaTestFiles.TestNewArray
  minor version: 0
  major version: 65
  flags: (0x0021) ACC_PUBLIC, ACC_SUPER
  this_class: #23                         // JavaTestFiles/TestNewArray
  super_class: #2                         // java/lang/Object
  interfaces: 0, fields: 0, methods: 2, attributes: 3
Constant pool:
   #1 = Methodref          #2.#3          // java/lang/Object."<init>":()V
   #2 = Class              #4             // java/lang/Object
   #3 = NameAndType        #5:#6          // "<init>":()V
   #4 = Utf8               java/lang/Object
   #5 = Utf8               <init>
   #6 = Utf8               ()V
   #7 = Fieldref           #8.#9          // java/lang/System.out:Ljava/io/PrintStream;
   #8 = Class              #10            // java/lang/System
   #9 = NameAndType        #11:#12        // out:Ljava/io/PrintStream;
  #10 = Utf8               java/lang/System
  #11 = Utf8               out
  #12 = Utf8               Ljava/io/PrintStream;
  #13 = InvokeDynamic      #0:#14         // #0:makeConcatWithConstants:(I)Ljava/lang/String;
  #14 = NameAndType        #15:#16        // makeConcatWithConstants:(I)Ljava/lang/String;
  #15 = Utf8               makeConcatWithConstants
  #16 = Utf8               (I)Ljava/lang/String;
  #17 = Methodref          #18.#19        // java/io/PrintStream.println:(Ljava/lang/String;)V
  #18 = Class              #20            // java/io/PrintStream
  #19 = NameAndType        #21:#22        // println:(Ljava/lang/String;)V
  #20 = Utf8               java/io/PrintStream
  #21 = Utf8               println
  #22 = Utf8               (Ljava/lang/String;)V
  #23 = Class              #24            // JavaTestFiles/TestNewArray
  #24 = Utf8               JavaTestFiles/TestNewArray
  #25 = Utf8               Code
  #26 = Utf8               LineNumberTable
  #27 = Utf8               main
  #28 = Utf8               ([Ljava/lang/String;)V
  #29 = Utf8               SourceFile
  #30 = Utf8               TestNewArray.java
  #31 = Utf8               BootstrapMethods
  #32 = String             #33            // Array length: \u0001
  #33 = Utf8               Array length: \u0001
  #34 = MethodHandle       6:#35          // REF_invokeStatic java/lang/invoke/StringConcatFactory.makeConcatWithConstants:(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/invoke/CallSite;
  #35 = Methodref          #36.#37        // java/lang/invoke/StringConcatFactory.makeConcatWithConstants:(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/invoke/CallSite;
  #36 = Class              #38            // java/lang/invoke/StringConcatFactory
  #37 = NameAndType        #15:#39        // makeConcatWithConstants:(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/invoke/CallSite;
  #38 = Utf8               java/lang/invoke/StringConcatFactory
  #39 = Utf8               (Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/invoke/CallSite;
  #40 = Utf8               InnerClasses
  #41 = Class              #42            // java/lang/invoke/MethodHandles$Lookup
  #42 = Utf8               java/lang/invoke/MethodHandles$Lookup
  #43 = Class              #44            // java/lang/invoke/MethodHandles
  #44 = Utf8               java/lang/invoke/MethodHandles
  #45 = Utf8               Lookup
{
  public JavaTestFiles.TestNewArray();
    descriptor: ()V
    flags: (0x0001) ACC_PUBLIC
    Code:
      stack=1, locals=1, args_size=1
         0: aload_0
         1: invokespecial #1                  // Method java/lang/Object."<init>":()V
         4: return
      LineNumberTable:
        line 3: 0

  public static void main(java.lang.String[]);
    descriptor: ([Ljava/lang/String;)V
    flags: (0x0009) ACC_PUBLIC, ACC_STATIC
    Code:
      stack=2, locals=2, args_size=1
         0: bipush        10
         2: newarray       int
         4: astore_1
         5: getstatic     #7                  // Field java/lang/System.out:Ljava/io/PrintStream;
         8: aload_1
         9: arraylength
        10: invokedynamic #13,  0             // InvokeDynamic #0:makeConcatWithConstants:(I)Ljava/lang/String;
        15: invokevirtual #17                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
        18: return
      LineNumberTable:
        line 5: 0
        line 6: 5
        line 7: 18
}
SourceFile: "TestNewArray.java"
BootstrapMethods:
  0: #34 REF_invokeStatic java/lang/invoke/StringConcatFactory.makeConcatWithConstants:(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/invoke/CallSite;
    Method arguments:
      #32 Array length: \u0001
InnerClasses:
  public static final #45= #41 of #43;    // Lookup=class java/lang/invoke/MethodHandles$Lookup of class java/lang/invoke/MethodHandles
