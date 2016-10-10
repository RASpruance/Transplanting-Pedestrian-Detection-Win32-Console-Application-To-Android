package com.bn.ex7f;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;


public class Sample7_6_Activity extends Activity {
    Button    button;
    ListView  listView;
    ImageView imageView;
    ArrayList<File> fileArrayList = new ArrayList<>();
    private              PedDetect peddetect = new PedDetect();
    private static final String    LOG_TAG   = "DATAFILE";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        button = (Button) this.findViewById(R.id.btpss);
        listView = (ListView) this.findViewById(R.id.lvtplb);
        imageView = (ImageView) this.findViewById(R.id.iv);
        String dataPath = "DetectionData";
        try {
            String[] dataFileList = getAssets().list(dataPath);
            if (0 < dataFileList.length)
                for (String filename : dataFileList) {
                    if (isFileType(filename, new String[]{"dat"})) {
                        InputStream dataFileInputStream = getAssets().open(dataPath + "/" + filename);
                        FileOutputStream dataFileOutputStream = new FileOutputStream(new File(
                                getString(R.string.DataDirectory) + "/" + filename));
                        byte[] buffer = new byte[1024];
                        int byteCount = 0;
                        while ((byteCount = dataFileInputStream.read(buffer)) != -1) //循环从输入流读取 buffer字节
                            dataFileOutputStream.write(buffer, 0, byteCount);//将读取的输入流写入到输出流
                        dataFileOutputStream.flush();//刷新缓冲区
                        dataFileInputStream.close();
                        dataFileOutputStream.close();
                    }
                }
        } catch (IOException e) {
            String ErrorInfo = "Cannot load Data Files.";
            // TODO: 2016/10/9 添加模型文件是否已存在的判别
            Log.e(LOG_TAG, ErrorInfo, new IOException(ErrorInfo));
        }

        File[] files = new File(getString(R.string.DataDirectory)).listFiles();
        if (null != files)
            for (File f : files) {
                if (!f.exists()) break;
                String fileName = f.getName();
                if (isFileType(fileName, new String[]{"png", "jpeg", "jpg"}))
                    fileArrayList.add(f);
            }

        button.setOnClickListener(
                new OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        BaseAdapter baseAdapter = new BaseAdapter() {
                            @Override
                            public int getCount() {
                                return fileArrayList.size();
                            }

                            @Override
                            public Object getItem(int position) {
                                return null;
                            }

                            @Override
                            public long getItemId(int position) {
                                return 0;
                            }

                            @Override
                            public View getView(int position, View convertView, ViewGroup parent) {
                                LinearLayout linearLayout = new LinearLayout(Sample7_6_Activity.this);
                                linearLayout.setOrientation(LinearLayout.VERTICAL);
                                linearLayout.setPadding(5, 5, 5, 5);
                                TextView textView = new TextView(Sample7_6_Activity.this);
                                textView.setTextColor(Color.BLACK);
                                textView.setText(fileArrayList.get(position).getName());
                                textView.setGravity(Gravity.LEFT);
                                textView.setTextSize(18);
                                linearLayout.addView(textView);
                                return linearLayout;
                            }
                        };
                        listView.setAdapter(baseAdapter);

                        listView.setOnItemClickListener(
                                new OnItemClickListener() {
                                    @Override
                                    public void onItemClick(AdapterView<?> parent,
                                                            View view,
                                                            int position,
                                                            long id) {
                                        synchronized (this) {
                                            File curfile = fileArrayList.get(position);
                                            String filePath = curfile.getPath();
                                            BitmapFactory.Options opts = new BitmapFactory.Options();
                                            opts.inMutable = true;
                                            Bitmap bitmap = BitmapFactory.decodeFile(
                                                    filePath, opts);
                                            int[] pixels = new int[bitmap.getWidth() * bitmap
                                                    .getHeight()];
                                            bitmap.getPixels(pixels,
                                                             0,
                                                             bitmap.getWidth(),
                                                             0,
                                                             0,
                                                             bitmap.getWidth(),
                                                             bitmap.getHeight());
                                            int[] resultData;

                                            long start_time = System.currentTimeMillis();

                                            resultData = peddetect.Start_Detect(
                                                    pixels,
                                                    bitmap.getWidth(),
                                                    bitmap.getHeight());

                                            long end_time = System.currentTimeMillis();
                                            long spend_time = end_time - start_time;
                                            Log.i("test","mView.draw: spend_time = " + spend_time);

                                            bitmap = peddetect.drawRectangles(bitmap, resultData);
                                            imageView.setImageBitmap(bitmap);
                                        }
                                    }
                                }
                                                       );
                    }
                }
                                 );
    }

    private boolean isFileType(String fileName, String[] exts) {
        String[] fileParts = fileName.split("\\.");
        if (fileParts.length == 2)
            for (String ext : exts)
                if (ext.toLowerCase().equals(fileParts[1].toLowerCase())) return true;
        return false;
    }
}
