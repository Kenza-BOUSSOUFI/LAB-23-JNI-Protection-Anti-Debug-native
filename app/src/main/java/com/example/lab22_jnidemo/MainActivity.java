package com.example.lab22_jnidemo;

import androidx.appcompat.app.AppCompatActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    public native boolean isDebugDetected();
    public native String helloFromJNI();
    public native int factorial(int n);

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tvStatus    = findViewById(R.id.tvStatus);
        TextView tvMessage   = findViewById(R.id.tvMessage);
        TextView tvFactoriel = findViewById(R.id.tvFactoriel);

        boolean environnementSuspect = isDebugDetected();

        if (environnementSuspect) {
            tvStatus.setText("Securite : environnement de debug detecte !");
            tvStatus.setTextColor(Color.RED);

            tvMessage.setText("Fonction native desactivee pour raison de securite");
            tvFactoriel.setText("Calcul natif bloque");
        } else {
            tvStatus.setText("Securite : environnement sain");
            tvStatus.setTextColor(Color.parseColor("#2E7D32"));

            tvMessage.setText(helloFromJNI());

            int fact7 = factorial(7);
            if (fact7 >= 0) {
                tvFactoriel.setText("Factoriel de 7 = " + fact7);
            } else {
                tvFactoriel.setText("Erreur lors du calcul factoriel");
            }
        }
    }
}