public class RaceConditionExample {
  private static int j;

  private static int k;

  public RaceConditionExample();
    Code:
       0: aload_0
       1: invokespecial #3                  // Method java/lang/Object."<init>":()V
       4: return

  public static void main(java.lang.String[]);
    Code:
       0: new           #4                  // class java/lang/Thread
       3: dup
       4: new           #5                  // class RaceConditionExample$1
       7: dup
       8: invokespecial #6                  // Method RaceConditionExample$1."<init>":()V
      11: invokespecial #7                  // Method java/lang/Thread."<init>":(Ljava/lang/Runnable;)V
      14: astore_1
      15: new           #4                  // class java/lang/Thread
      18: dup
      19: new           #8                  // class RaceConditionExample$2
      22: dup
      23: invokespecial #9                  // Method RaceConditionExample$2."<init>":()V
      26: invokespecial #7                  // Method java/lang/Thread."<init>":(Ljava/lang/Runnable;)V
      29: astore_2
      30: aload_1
      31: invokevirtual #10                 // Method java/lang/Thread.start:()V
      34: aload_2
      35: invokevirtual #10                 // Method java/lang/Thread.start:()V
      38: aload_1
      39: invokevirtual #11                 // Method java/lang/Thread.join:()V
      42: aload_2
      43: invokevirtual #11                 // Method java/lang/Thread.join:()V
      46: goto          54
      49: astore_3
      50: aload_3
      51: invokevirtual #13                 // Method java/lang/InterruptedException.printStackTrace:()V
      54: getstatic     #14                 // Field java/lang/System.out:Ljava/io/PrintStream;
      57: new           #15                 // class java/lang/StringBuilder
      60: dup
      61: invokespecial #16                 // Method java/lang/StringBuilder."<init>":()V
      64: ldc           #17                 // String Final values: j =
      66: invokevirtual #18                 // Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
      69: getstatic     #2                  // Field j:I
      72: invokevirtual #19                 // Method java/lang/StringBuilder.append:(I)Ljava/lang/StringBuilder;
      75: ldc           #20                 // String , k =
      77: invokevirtual #18                 // Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
      80: getstatic     #1                  // Field k:I
      83: invokevirtual #19                 // Method java/lang/StringBuilder.append:(I)Ljava/lang/StringBuilder;
      86: invokevirtual #21                 // Method java/lang/StringBuilder.toString:()Ljava/lang/String;
      89: invokevirtual #22                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
      92: return
    Exception table:
       from    to  target type
          38    46    49   Class java/lang/InterruptedException

  static int access$002(int);
    Code:
       0: iload_0
       1: dup
       2: putstatic     #2                  // Field j:I
       5: ireturn

  static int access$102(int);
    Code:
       0: iload_0
       1: dup
       2: putstatic     #1                  // Field k:I
       5: ireturn

  static int access$008();
    Code:
       0: getstatic     #2                  // Field j:I
       3: dup
       4: iconst_1
       5: iadd
       6: putstatic     #2                  // Field j:I
       9: ireturn

  static int access$108();
    Code:
       0: getstatic     #1                  // Field k:I
       3: dup
       4: iconst_1
       5: iadd
       6: putstatic     #1                  // Field k:I
       9: ireturn

  static int access$000();
    Code:
       0: getstatic     #2                  // Field j:I
       3: ireturn

  static int access$100();
    Code:
       0: getstatic     #1                  // Field k:I
       3: ireturn

  static int access$010();
    Code:
       0: getstatic     #2                  // Field j:I
       3: dup
       4: iconst_1
       5: isub
       6: putstatic     #2                  // Field j:I
       9: ireturn

  static int access$110();
    Code:
       0: getstatic     #1                  // Field k:I
       3: dup
       4: iconst_1
       5: isub
       6: putstatic     #1                  // Field k:I
       9: ireturn

  static {};
    Code:
       0: iconst_0
       1: putstatic     #2                  // Field j:I
       4: iconst_0
       5: putstatic     #1                  // Field k:I
       8: return
}