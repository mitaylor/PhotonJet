#include <iostream>

int first_sig_digit(double X);
int count_digit(double X, int D);
int count_digit_below(int D);
double round(double X, int D);

int first_sig_digit(double X)
{
   if(X < 0)
      X = -X;

   if(X == 0)
      return -999;
   if(X < 10 && X >= 1)
      return 0;

   if(X >= 1)
   {
      int Count = 0;
      while(X >= 10)
      {
         X = X / 10;
         Count = Count + 1;
      }

      return Count;
   }

   int Count = 0;
   while(X < 1)
   {
      X = X * 10;
      Count = Count - 1;
   }

   return Count;
}

int count_digit(double X, int D)
{
   return first_sig_digit(X) - D + 1;
}

int count_digit_below(int D)
{
   if(D >= 0)
      return 0;
   return -D;
}

double round(double X, int D)
{
   bool Negative = false;
   if(X < 0)
   {
      Negative = true;
      X = -X;
   }

   if(D >= 0)
   {
      for(int i = 0; i < D; i++)
         X = X / 10;

      int IntX = X;
      if(X - IntX >= 0.5)
         X = IntX + 1;
      else
         X = IntX;
      
      for(int i = 0; i < D; i++)
         X = X * 10;
   }
   else
   {
      for(int i = 0; i > D; i--)
         X = X * 10;

      int IntX = X;
      if(X - IntX >= 0.5)
         X = IntX + 1;
      else
         X = IntX;

      for(int i = 0; i > D; i--)
         X = X / 10;
   }

   if(Negative == true)
      X = -X;
   return X;
}