package com.bn.ex7f;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;

public class PedDetect {

    static {
        System.loadLibrary("PedDetect");
    }

    public Bitmap drawRectangles(Bitmap imageBitmap, int[] Result) {
        int left, top, right, bottom;
        Bitmap mutableBitmap;
        if (imageBitmap.isMutable())
            mutableBitmap = imageBitmap;
        else
            mutableBitmap = imageBitmap.copy(Bitmap.Config.ARGB_8888, true);
        Canvas canvas = new Canvas(mutableBitmap);
        Paint paint = new Paint();
        int NumOfBoundingBoxes = Result[0];
        Log.d("test", "Length of Result is" + Result.length);
        for (int i = 1; i < NumOfBoundingBoxes + 1; i++) {
            left = Result[4 * i - 3];
            top = Result[4 * i - 2];
            right = Result[4 * i - 1];
            bottom = Result[4 * i];
            paint.setColor(Color.RED);
            paint.setStyle(Paint.Style.STROKE);//不填充
            paint.setStrokeWidth(10);  //线的宽度
            canvas.drawRect(left, top, right, bottom, paint);
        }
        return mutableBitmap;
    }

    public native int[] Start_Detect(int[] srcbuf, int width, int height);

}