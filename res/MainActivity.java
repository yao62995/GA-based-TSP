package com.example.applog_demo2;

import android.Manifest;
import android.annotation.TargetApi;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

import java.util.Collections;
import java.util.Arrays;

import com.bytedance.common.utility.NetworkClient;
import com.ss.android.common.AppContext;
import com.ss.android.common.applog.AppLog;
import com.ss.android.common.applog.TeaAgent;
import com.bytedance.article.common.network.SSNetworkClient;
import com.ss.android.common.applog.TeaConfig;
import com.ss.android.common.applog.TeaConfigBuilder;
import com.ss.android.common.applog.UrlConfig;

import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;

import java.util.HashMap;

public class MainActivity extends AppCompatActivity {
  private final int MY_PERMISSIONS_REQUEST_RECORD_AUDIO = 1;
  private final int MY_PERMISSIONS_REQUEST_READ_PHONE_STATE = 2;
  private Context context;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    Toolbar toolbar = findViewById(R.id.toolbar);
    setSupportActionBar(toolbar);

    context = this.getApplicationContext();


    NetworkClient.setDefault(new SSNetworkClient());
    TeaConfig teaConfig = teaConfigBuilder();
    TeaAgent.init(teaConfig);

    FloatingActionButton fab = findViewById(R.id.fab);
    fab.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
            .setAction("Action", null).show();
      }
    });

    System.out.println("begin to get device info");

    HashMap<String, String> body = getDeviceInfo(true);
    System.out.println(Arrays.asList(body)); // method 1
    System.out.println(Collections.singletonList(body)); // method 2
  }


  private HashMap<String, String> getDeviceInfo(boolean need_did) {
    HashMap<String, String> body = new HashMap<>();
    body.put("status", "ok");
    body.put("resource_dir", this.getApplicationContext().getFilesDir().getAbsolutePath());
    // get device id
    String device_id = "", install_id = "";
    if (checkReadPhonePermissions()) {
      int cnt = 0;
      while (cnt < 3) {
        TeaAgent.tryWaitDeviceInit();
        device_id = TeaAgent.getServerDeviceId();
        install_id = TeaAgent.getInstallId();
        if (device_id != null) {
          if (!device_id.equals("")) {
            break;
          }
        }
        cnt += 1;
      }
    }
    System.out.println("device id: " + device_id + ", install_id: " + install_id);
    body.put("device_id", device_id == null? "": device_id);
    body.put("install_id", install_id == null? "": install_id);
    return body;
  }

  private TeaConfig teaConfigBuilder() {
    return TeaConfigBuilder.create(this, true, UrlConfig.CHINA, new AppContext() {
      @Override
      public Context getContext() {
        //return getApplicationContext();
        return context;
      }

      @Override
      public String getStringAppName() {
        return "听头条";
      }

      @Override
      public String getAppName() {
        return "readtt";
      }

      @Override
      public String getVersion() {
        try {
          String pkName = getContext().getPackageName();
          return getContext().getPackageManager().getPackageInfo(pkName, 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
          e.printStackTrace();
          return null;
        }
      }

      @Override
      public String getFeedbackAppKey() {
        return null;
      }

      @Override
      public String getChannel() {
        return "bytedance_internal_test";
      }

      @Override
      public String getTweakedChannel() {
        return getChannel();
      }

      @Override
      public int getVersionCode() {
        try {
          String pkName = getContext().getPackageName();
          return getContext().getPackageManager().getPackageInfo(pkName, 0).versionCode;
        } catch (PackageManager.NameNotFoundException e) {
          e.printStackTrace();
          return 0;
        }
      }

      @TargetApi(26)
      @Override
      public String getDeviceId() {
        TelephonyManager telephonyManager = (TelephonyManager) getContext().getSystemService(Context.TELEPHONY_SERVICE);
        if (checkSelfPermission(Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
          Log.d("MainActivity", "read phone state permission rejected.");
          return "";
        }
        return telephonyManager.getImei();
      }

      @Override
      public int getUpdateVersionCode() {
        return 0;
      }

      @Override
      public int getManifestVersionCode() {
        return getVersionCode();
      }

      @Override
      public String getManifestVersion() {
        return getVersion();
      }

      @Override
      public int getAid() {
        return 2304;
      }

      @Override
      public String getAbClient() {
        return null;
      }

      @Override
      public long getAbFlag() {
        return 0;
      }

      @Override
      public String getAbVersion() {
        return null;
      }

      @Override
      public String getAbGroup() {
        return null;
      }

      @Override
      public String getAbFeature() {
        return null;
      }
    })
        .setEncryptConfig(new AppLog.ILogEncryptConfig() {
          @Override
          public boolean getEncryptSwitch() {
            return true;
          }

          @Override
          public boolean getEventV3Switch() {
            return true;
          }

          @Override
          public boolean getRecoverySwitch() {
            return true;
          }
        }).build();
  }


  private boolean checkReadPhonePermissions() {
    //int permissionCheck = ContextCompat.checkSelfPermission(activity,
    //    Manifest.permission.RECORD_AUDIO);
    //return permissionCheck == PackageManager.PERMISSION_GRANTED;

    // Here, thisActivity is the current activity
    if (ContextCompat.checkSelfPermission(this,
        Manifest.permission.READ_PHONE_STATE)
        != PackageManager.PERMISSION_GRANTED) {

      // Permission is not granted
      // Should we show an explanation?
      if (ActivityCompat.shouldShowRequestPermissionRationale(this,
          Manifest.permission.READ_PHONE_STATE)) {
        // Show an explanation to the user *asynchronously* -- don't block
        // this thread waiting for the user's response! After the user
        // sees the explanation, try again to request the permission.
        return false;
      } else {
        // No explanation needed; request the permission
        ActivityCompat.requestPermissions(this,
            new String[]{Manifest.permission.READ_PHONE_STATE},
            MY_PERMISSIONS_REQUEST_READ_PHONE_STATE);

        // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
        // app-defined int constant. The callback method gets the
        // result of the request.
        return false;
      }
    } else {
      return true;
    }
  }


  @Override
  public void onRequestPermissionsResult(int requestCode,
                                         String[] permissions, int[] grantResults) {
    switch (requestCode) {
      case MY_PERMISSIONS_REQUEST_RECORD_AUDIO: {
        // If request is cancelled, the result arrays are empty.
        if (grantResults.length > 0
            && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
          HashMap<String, Object> body = new HashMap<>();
          body.put("code", "permission_record");
          body.put("status", true);
          System.out.println(Arrays.asList(body)); // method 1
          System.out.println(Collections.singletonList(body)); // method 2
        }
      }
      case MY_PERMISSIONS_REQUEST_READ_PHONE_STATE: {
        // If request is cancelled, the result arrays are empty.
        if (grantResults.length > 0
            && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
          // send event
          HashMap<String, String> body = getDeviceInfo(true);
          body.put("code", "permission_phone");
          System.out.println("success to get permission: permission_phone");
          System.out.println(Arrays.asList(body)); // method 1
          System.out.println(Collections.singletonList(body)); // method 2
        }
      }
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.menu_main, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    // Handle action bar item clicks here. The action bar will
    // automatically handle clicks on the Home/Up button, so long
    // as you specify a parent activity in AndroidManifest.xml.
    int id = item.getItemId();

    //noinspection SimplifiableIfStatement
    if (id == R.id.action_settings) {
      return true;
    }

    return super.onOptionsItemSelected(item);
  }
}
