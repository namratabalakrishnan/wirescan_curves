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

int sizeVel = 1;

/* Scan Range per wire in mm. Order of the wires: u, x, y */
double wireRange[3][2] = { 
                           {14.16, 18.16},/*U wire INNER and OUTER*/ 
                           {28.81, 32.81},/*X wire INNER and OUTER*/  
                           {38.32, 42.32} /*Y wire INNER and OUTER*/ 
                         }; 

double getSpeed (double repRate) {
  
  double desSpeed = repRate * scanDist / desPoints;
  double min = MIN(maxSpeed,desSpeed);
  return min  ;
}   

double * build_velocity_profile( double speed, double maxSpeed, double dt) {
   
   // Profile points defining the shape of the scan
   double scanPoints[7] = {0};
   
   scanPoints[0]=wireRange[0][0];
   scanPoints[2]=wireRange[1][0] - wireRange[0][1];
   scanPoints[4]=wireRange[2][0] - wireRange[1][1];
   scanPoints[1]=scanPoints[3]=scanPoints[5]= scanDist;
   scanPoints[6]=wireRange[2][1];
  
   printf("This is the distance per speed according to the profile to be generated: \n");
   int i;
   
   for (i=0;i < 7 ;i++) {
       printf("%f\n",scanPoints[i]);
   }
   
   // Profile speed defining the permissible speed along the scan points
   double speeds[7] = {0};
   
   speeds[0]=speeds[2]=speeds[4]=speeds[6]=maxSpeed;
   speeds[1]=speeds[3]=speeds[5]=speed;
   
   printf("This is the profile speed : \n");
   for (i=0;i < 7 ;i++) {
       printf("%f\n",speeds[i]);
   }
 
   double *pNewVelocity; 
   double *temp;
   pNewVelocity= (double *)malloc((size_t)((1)*sizeof(double)));  
 
   FILE * fp;
   int a = 0;
   int b = 6;
   
   printf("This is the profile velocity : \n");
   int bit; 
  
   int dummy[7]= {0};
   
   sizeVel = 0;
 
   for (i=0;i < 7 ;i++) {
       double dt0 = (scanPoints[i] / speeds[i]);
       printf(" scanPoints = %0.3f speed = %0.3f dt0 = %0.3f\n ",scanPoints[i],speeds[i],dt0);
       double m = dt0/dt;
       printf(" dt = %0.3f m = %0.3f round m = %0.3f\n ",dt,m,round(m));
       sizeVel = sizeVel + (int)m;
       printf("sizeVel = %d\n",sizeVel);

       dummy[i] = sizeVel;
       printf("dummy = %d\n",dummy[i]);
   }
  
  temp = (double *)realloc(pNewVelocity, sizeVel*sizeof(double));
  
  i = 0;
  for (bit =0;bit < sizeVel;bit++) {
    if(bit == dummy[i])
       i++;
         
    *(temp+bit) = speeds[i];
    if(i == 6)
    *(temp+bit) = -speeds[i]; 
  
    fp= fopen("/u/cd/namrata/workspace/git-repos/wirescan_curves/scanData.csv","a");
    fprintf(fp,"%0.3f\n",*(temp+bit));
    fclose(fp);
    }
  
   return temp;
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
}


       
 
       
main() {

  double speed;

  double dt = 0.01;
  int smoothing = 3;
  
  double repRate = 100;
  
  double *PvelProf;
  speed = getSpeed(repRate);

  printf("Minimum speed = %f \n ", speed);
  
  PvelProf = build_velocity_profile(speed, maxSpeed, dt);

  build_position_profile(dt, PvelProf, smoothing);  
  free(PvelProf);
}

