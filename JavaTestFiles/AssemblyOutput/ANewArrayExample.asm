Compiled from "ANewArrayExample.java"
public class ANewArrayExample {
  public ANewArrayExample();
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  public static void main(java.lang.String[]);
    Code:
       0: iconst_5
       1: anewarray     #7                  // class java/lang/String
       4: astore_1
       5: aload_1
       6: iconst_0
       7: ldc           #9                  // String Hello
       9: aastore
      10: aload_1
      11: iconst_1
      12: ldc           #11                 // String ANewArray
      14: aastore
      15: aload_1
      16: iconst_2
      17: ldc           #13                 // String Opcode
      19: aastore
      20: aload_1
      21: iconst_3
      22: ldc           #15                 // String In
      24: aastore
      25: aload_1
      26: iconst_4
      27: ldc           #17                 // String Java
      29: aastore
      30: aload_1
      31: astore_2
      32: aload_2
      33: arraylength
      34: istore_3
      35: iconst_0
      36: istore        4
      38: iload         4
      40: iload_3
      41: if_icmpge     64
      44: aload_2
      45: iload         4
      47: aaload
      48: astore        5
      50: getstatic     #19                 // Field java/lang/System.out:Ljava/io/PrintStream;
      53: aload         5
      55: invokevirtual #25                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      58: iinc          4, 1
      61: goto          38
      64: return
}
