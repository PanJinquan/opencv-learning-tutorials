package com.panjq.opencv.opencvdemo;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

import static com.panjq.opencv.opencvdemo.LogUtil.TAG;

public class CopyFileTask extends AsyncTask<Void,Void,Void> {


    // copy file task file
//    public   static String model_path = Environment.getExternalStorageDirectory().getAbsolutePath()+"/cube/88.cube";
    public   static String file_path1 = Environment.getExternalStorageDirectory().getAbsolutePath()+"/yolov3/yolov3.weights";
    public   static String file_path2 = Environment.getExternalStorageDirectory().getAbsolutePath()+"/yolov3/yolov3.cfg";
    public   static String file_path3 = Environment.getExternalStorageDirectory().getAbsolutePath()+"/yolov3/coco.names";

    private Context context;
    private TaskListener taskListener;
   public CopyFileTask( Context context, TaskListener taskListener){
        this.context = context;
        this.taskListener = taskListener;
    }

    @Override
    protected Void doInBackground(Void... voids) {
        List<File> list = new ArrayList<>();
        list.add(new File(file_path1));
        list.add(new File(file_path2));
        list.add(new File(file_path3));

        for(int i = 0; i< list.size();i++) {
            File file = list.get(i);
            if(!file.exists() && !file.getParentFile().exists())
            {
                file.getParentFile().mkdirs();
            }else{
                try {
                    copyBigDataToSD(context,file.getName(), file.getAbsolutePath());
                    Log.e(TAG, " Copy file="+ file.getAbsolutePath());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

        }
        return null;
    }

    @Override
    protected void onPostExecute(Void aVoid) {
       if(taskListener != null){
           taskListener.onTaskDone();
       }

    }


    private void copyBigDataToSD(Context context, String fileName, String strOutFileName) throws Exception {
        InputStream myInput;
        OutputStream myOutput = new FileOutputStream(strOutFileName);
        myInput = context.getAssets().open(fileName);

        byte[] buffer = new byte[1024];
        int length = myInput.read(buffer);
        while (length > 0) {
            myOutput.write(buffer, 0, length);
            length = myInput.read(buffer);
        }

        myOutput.flush();
        myInput.close();
        myOutput.close();
    }



    public void setTaskDoneLiteneter(TaskListener taskListener) {
        this.taskListener = taskListener;
    }


    public interface TaskListener{
        void onTaskStart();
        void onTaskDone();
    }
}
