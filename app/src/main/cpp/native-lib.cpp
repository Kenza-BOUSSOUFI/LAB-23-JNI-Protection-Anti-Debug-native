#include <jni.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <android/log.h>
#include <sys/ptrace.h>
#include <unistd.h>

#define TAG "LAB22_ANTI_DEBUG"
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOG_ERR(...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// --------------------------------------------------
// Contrôle 1 : détection de traçage via ptrace
// --------------------------------------------------
static bool isBeingTraced() {
    long resultat = ptrace(PTRACE_TRACEME, 0, 0, 0);
    if (resultat == -1) {
        LOG_ERR("Etat suspect : processus en cours de trace");
        return true;
    }
    LOG_INFO("ptrace : aucun tracage detecte");
    return false;
}

// --------------------------------------------------
// Contrôle 2 : recherche de signatures suspectes
//              dans /proc/self/maps
// --------------------------------------------------
static bool containsSuspiciousLibraryNames() {
    FILE* fichierMaps = fopen("/proc/self/maps", "r");
    if (!fichierMaps) {
        LOG_WARN("Lecture de /proc/self/maps impossible");
        return false;
    }

    char ligne[512];

    while (fgets(ligne, sizeof(ligne), fichierMaps)) {
        if (strstr(ligne, "frida")     ||
            strstr(ligne, "xposed")    ||
            strstr(ligne, "libfrida")  ||
            strstr(ligne, "gdbserver") ||
            strstr(ligne, "libgdb")    ||
            strstr(ligne, "magisk")) {
            LOG_ERR("Bibliotheque suspecte detectee : %s", ligne);
            fclose(fichierMaps);
            return true;
        }
    }

    fclose(fichierMaps);
    LOG_INFO("maps : aucune bibliotheque suspecte trouvee");
    return false;
}

// --------------------------------------------------
// Contrôle global : appelé depuis Java
// --------------------------------------------------
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_lab22_1jnidemo_MainActivity_isDebugDetected(
        JNIEnv* env,
        jobject /* instance */) {

    bool traced          = isBeingTraced();
    bool suspiciousMaps  = containsSuspiciousLibraryNames();

    if (traced || suspiciousMaps) {
        LOG_ERR("Securite : environnement de debug ou instrumentation detecte");
        return JNI_TRUE;
    }

    LOG_INFO("Securite : environnement sain, aucune menace detectee");
    return JNI_FALSE;
}

// --------------------------------------------------
// Fonctions JNI du laboratoire précédent
// --------------------------------------------------
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_lab22_1jnidemo_MainActivity_helloFromJNI(
        JNIEnv* env,
        jobject /* instance */) {
    LOG_INFO("helloFromJNI invoquee depuis le code natif");
    return env->NewStringUTF("Bonjour depuis C++ via JNI !");
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_lab22_1jnidemo_MainActivity_factorial(
        JNIEnv* env,
        jobject /* instance */,
        jint nombre) {

    if (nombre < 0) {
        LOG_ERR("Valeur invalide : nombre negatif recu");
        return -1;
    }

    long long resultat = 1;
    for (int i = 1; i <= nombre; i++) {
        resultat *= i;
    }

    LOG_INFO("Factoriel(%d) = %lld", nombre, resultat);
    return static_cast<jint>(resultat);
}