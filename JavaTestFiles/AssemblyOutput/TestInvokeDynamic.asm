Compiled from "TestInvokeDynamic.java"
public class TestInvokeDynamic {
  public TestInvokeDynamic();
    Code:
       0: aload_0
       1: invokespecial #1                  // Method java/lang/Object."<init>":()V
       4: return

  public static void main(java.lang.String[]);
    Code:
       0: getstatic     #7                  // Field java/lang/System.out:Ljava/io/PrintStream;
       3: dup
       4: invokestatic  #13                 // Method java/util/Objects.requireNonNull:(Ljava/lang/Object;)Ljava/lang/Object;
       7: pop
       8: invokedynamic #19,  0             // InvokeDynamic #0:accept:(Ljava/io/PrintStream;)Ljava/util/function/Consumer;
      13: astore_1
      14: aload_1
      15: ldc           #23                 // String Hello, invokedynamic!
      17: invokeinterface #25,  2           // InterfaceMethod java/util/function/Consumer.accept:(Ljava/lang/Object;)V
      22: return
}
