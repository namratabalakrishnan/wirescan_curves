#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MIN( a, b ) ( ( a < b) ? a : b ) 

/* Max speed at which the stage can move mm/sec? */
double maxSpeed = 100;
/* Distance scanner over the wire in mm*/
double scanDist = 4.0;
/* Number of points desired. From the Physics requrements document : < Insert revision > */
double desPoints = 100;

/* Size of the velocity profile after realloc*/
int sizeVel = 1;

/* Array to hold profile chsange points */
int dummy[7]= {0};

/* Scan Range per wire in mm. Order of the wires: u, x, y */
double wireRange[3][2] = { 
                           {14.16, 18.16},/*U wire INNER and OUTER*/ 
                           {28.81, 32.81},/*X wire INNER and OUTER*/  
                           {38.32, 42.32} /*Y wire INNER and OUTER*/ 
                         }; 

/*Selects the maximum speed according to the repRate*/
double getSpeed (double repRate) {
  
  double desSpeed = repRate * scanDist / desPoints;
  double min = MIN(maxSpeed,desSpeed);
  return min  ;
}   

/* Running Average*/
double * convolve(double* pSignal, size_t SignalLen,size_t KernelLen) {
  
  int window = 5;

  size_t n;
  double *pResult;
  pResult= (double *)malloc((size_t)((SignalLen + KernelLen -1)*sizeof(double)));
  FILE * fp;

  double kernel[KernelLen];
   for (n = 0; n< KernelLen; n++){
       kernel[n] = 0.4;
   }

  for (n = 0; n < SignalLen + KernelLen - 1; n++) {
    size_t kmin, kmax, k;

    *(pResult+n) = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++) {
      *(pResult+n) += *(pSignal+k) * kernel[n - k];
    }
    fp= fopen("/u/cd/namrata/workspace/git-repos/wirescan_curves/scanDataResult.csv","a");
    fprintf(fp,"%0.3f\n",*(pResult+n));
    fclose(fp);
   //printf("Smoothed curve a= %f\n", *(pResult+n));
  }

  return pResult;
}






/* Builds the velocity profile */
double * build_velocity_profile( double speed, double maxSpeed, double dt) {
   
   // Profile points defining the shape of the scan
   double scanPoints[7] = {0};
   
   scanPoints[0]=wireRange[0][0];
   scanPoints[2]=wireRange[1][0] - wireRange[0][1];
   scanPoints[4]=wireRange[2][0] - wireRange[1][1];
   scanPoints[1]=scanPoints[3]=scanPoints[5]= scanDist;
   scanPoints[6]=wireRange[2][1];
  
   int i;

   // Profile speed defining the permissible speed along the scan points
   double speeds[7] = {0};
   
   speeds[0]=speeds[2]=speeds[4]=speeds[6]=maxSpeed;
   speeds[1]=speeds[3]=speeds[5]=speed;   
 
   double *pNewVelocity; 
   double *pVelProfile;
   pNewVelocity= (double *)malloc((size_t)((1)*sizeof(double)));  
 
   FILE * fp;
   
   int bit; 
   
   sizeVel = 0;
 
   for (i=0;i < 7 ;i++) {
       double dt0 = (scanPoints[i] / speeds[i]);
       double m = dt0/dt;
       sizeVel = sizeVel + (int)m;
       dummy[i] = sizeVel;
   }
  
  pVelProfile = (double *)realloc(pNewVelocity, sizeVel*sizeof(double));
  
  i = 0;
  for (bit =0;bit < sizeVel;bit++) {
    if(bit == dummy[i])
       i++;
         
    *(pVelProfile+bit) = speeds[i];
    if(i == 6)
    *(pVelProfile+bit) = -speeds[i]; 
  
    fp= fopen("/u/cd/namrata/workspace/git-repos/wirescan_curves/scanData.csv","a");
    fprintf(fp,"%0.3f\n",*(pVelProfile+bit));
    fclose(fp);
    }
  
   return pVelProfile;
}


void build_position_profile (double dt, double* velProfile, int smoothing){

   double count = 0.0;
   double time[371]= {0};
   int i;
   double *pNewPosition;
   FILE * fp;

   pNewPosition= (double *)malloc((size_t)((sizeVel)*sizeof(double)));

   double *PResult;

   for (i =0; i<sizeVel ; i++){
      *(pNewPosition+i) = count + *(velProfile+i) * dt;
      count= *(pNewPosition+i);
         fp= fopen("/u/cd/namrata/workspace/git-repos/wirescan_curves/scanDataPos.csv","a");
         fprintf(fp,"%0.3f\n",*(pNewPosition+i));
         fclose(fp);
   }

   PResult = convolve(pNewPosition,sizeVel,smoothing);
}


       
 
       
main() {

  double speed;

  double dt = 0.01;
  int smoothing = 3;
  
  double repRate = 100;
  
  double *pvelProfile;
  speed = getSpeed(repRate);
  
  pvelProfile = build_velocity_profile(speed, maxSpeed, dt);

  build_position_profile(dt, pvelProfile, smoothing);  
  free(pvelProfile);
}

