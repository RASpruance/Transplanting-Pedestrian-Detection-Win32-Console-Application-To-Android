
class PedDetect {

    //
    static {
        System.loadLibrary("PedDetect");
    }


    public native void Initialization();


    public native int[] Start_Detect(int[] srcbuf);

}