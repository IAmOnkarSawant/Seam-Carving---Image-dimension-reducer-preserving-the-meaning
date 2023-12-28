#include <iostream>
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    // Load the input image
    Mat inputImage = imread(argv[1]);
    if (inputImage.empty())
    {
        cout << "Could not read the image!!" << endl;
        return -1;
    }

    int rows = inputImage.rows;
    int cols = inputImage.cols;
    int o_rows = rows;
    int o_cols = cols;
    int expected_rows, expected_cols;
    //cout << rows << " - " << cols << "\n";
    cin >> expected_rows >> expected_cols;
	
	//Vec3b rgbPixel = (int)inputImage.at<Vec3b>(i, j);


	//*****************************************************************************
	//		         	Calculation of energy matrix 
	//*****************************************************************************
	
    float **energyMatrix = new float*[rows];
	for (int i = 0; i < rows; ++i) 
	{
		energyMatrix[i] = new float[cols];
	}
    
    //float energyRowMatrixDP[rows+1][cols+1];
    //float energyColMatrixDP[rows+1][cols+1];
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int l = (j == 0) ? (cols-1) : (j-1);
            int r = (j == (cols-1)) ? 0 : (j+1);
            int u = (i == 0) ? (rows-1) : (i-1);
            int d = (i == (rows-1)) ? 0 : (i+1);
           
            int temp = abs((int)inputImage.at<Vec3b>(i, l)[0] - (int)inputImage.at<Vec3b>(i, r)[0]);
            int Xgredient = temp * temp;
            temp = abs((int)inputImage.at<Vec3b>(i, l)[1] - (int)inputImage.at<Vec3b>(i, r)[1]);
            Xgredient += temp * temp;
            temp = abs((int)inputImage.at<Vec3b>(i, l)[2] - (int)inputImage.at<Vec3b>(i, r)[2]);
            Xgredient += temp * temp;

            temp = abs((int)inputImage.at<Vec3b>(u, j)[0] - (int)inputImage.at<Vec3b>(d, j)[0]);
            int Ygredient = temp * temp;
            temp = abs((int)inputImage.at<Vec3b>(u, j)[1] - (int)inputImage.at<Vec3b>(d, j)[1]);
            Ygredient += temp * temp;
            temp = abs((int)inputImage.at<Vec3b>(u, j)[2] - (int)inputImage.at<Vec3b>(d, j)[2]);
            Ygredient += temp * temp;

            int total = Xgredient + Ygredient;

            energyMatrix[i][j] = sqrt((float)total);
        }
    }
    
    //*****************************************************************************
	//		         	Seam carving on width if asked
	//*****************************************************************************
  
	if(expected_cols < cols)
    {
        float **energyRowMatrixDP = new float*[rows];
		for (int i = 0; i < rows; ++i) 
		{
    		energyRowMatrixDP[i] = new float[cols];
		}
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if(i == 0)
                {
                    energyRowMatrixDP[i][j] = energyMatrix[i][j];
                }
                else
                {
                    float ul,u,ur;
                    ul = energyRowMatrixDP[i-1][(j == 0)?cols-1:j-1];
                    u  = energyRowMatrixDP[i-1][(j)];
                    ur = energyRowMatrixDP[i-1][(j == cols-1)?0:j+1];
                    
                    float min_energy = min(ur, min(ul, u));
                    energyRowMatrixDP[i][j] = min_energy + energyMatrix[i][j];
                }
            }
        }               

        
        
	    while (expected_cols < cols)
	    {
	    	float min_Last_Row = INT_MAX; 
		    int min_Last_Row_index = INT_MAX; 
		    for (int i = 0; i < cols; i++)
		    {
		        if( min_Last_Row > energyRowMatrixDP[rows-1][i])
		        {
		            min_Last_Row = energyRowMatrixDP[rows-1][i];
		            min_Last_Row_index = i;
		        }
		    }
	    	for (int i = rows - 1; i > 0; i--)
		    {
		        if(i == rows-1)
		        {
		            for (int j = min_Last_Row_index; j < cols-1; j++)
		            {
		                inputImage.at<Vec3b>(i, j) = inputImage.at<Vec3b>(i, j+1);
		                energyRowMatrixDP[i][j] = energyRowMatrixDP[i][j+1];
		            }
		        }
		        else
		        {
		            float ul,u,ur;
		            if(min_Last_Row_index==0)
		            {
		                ul = INT_MAX;
		                u  = energyRowMatrixDP[i-1][(min_Last_Row_index)];
		                ur = energyRowMatrixDP[i-1][(min_Last_Row_index == cols-1)?0:min_Last_Row_index+1];
		            }
		            else if( min_Last_Row_index == cols-1)
		            {
		                ul = energyRowMatrixDP[i-1][(min_Last_Row_index == 0)?cols-1:min_Last_Row_index-1];
		                u  = energyRowMatrixDP[i-1][(min_Last_Row_index)];
		                ur = INT_MAX;
		            }
		            else
		            {
		                ul = energyRowMatrixDP[i-1][(min_Last_Row_index == 0)?cols-1:min_Last_Row_index-1];
		                u  = energyRowMatrixDP[i-1][(min_Last_Row_index)];
		                ur = energyRowMatrixDP[i-1][(min_Last_Row_index == cols-1)?0:min_Last_Row_index+1];
		            }
		            
		            if(ul < u && ul < ur)
		                min_Last_Row_index = min_Last_Row_index-1;
		            else if(u < ul && u < ur)
		                min_Last_Row_index = min_Last_Row_index;
		            else 
		                min_Last_Row_index = min_Last_Row_index+1;

		            for (int j = min_Last_Row_index; j < cols-1; j++)
		            {
		                inputImage.at<Vec3b>(i, j) = inputImage.at<Vec3b>(i, j+1);
		                energyRowMatrixDP[i][j] = energyRowMatrixDP[i][j+1];
		            }
		        } 
			}
			cols--;
        }  
            
    }
   

    Mat outputImage(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            outputImage.at<Vec3b>(i, j) = inputImage.at<Vec3b>(i, j);
        }
    }
    /*imwrite("outputcol1.jpeg", outputImage);*/

    
    //cout<<"outside row seam";
	if(expected_rows < rows)
    {
    	float **energyMatrixc = new float*[rows];
		for (int i = 0; i < rows; ++i) 
		{

    		energyMatrixc[i] = new float[cols];
		}
		for (int i = 0; i < rows; i++)
		{
		    for (int j = 0; j < cols; j++)
		    {
		        int l = (j == 0) ? (cols-1) : (j-1);
		        int r = (j == (cols-1)) ? 0 : (j+1);
		        int u = (i == 0) ? (rows-1) : (i-1);
		        int d = (i == (rows-1)) ? 0 : (i+1);
		       
		        int temp = abs((int)inputImage.at<Vec3b>(i, l)[0] - (int)inputImage.at<Vec3b>(i, r)[0]);
		        int Xgredient = temp * temp;
		        temp = abs((int)inputImage.at<Vec3b>(i, l)[1] - (int)inputImage.at<Vec3b>(i, r)[1]);
		        Xgredient += temp * temp;
		        temp = abs((int)inputImage.at<Vec3b>(i, l)[2] - (int)inputImage.at<Vec3b>(i, r)[2]);
		        Xgredient += temp * temp;

		        temp = abs((int)inputImage.at<Vec3b>(u, j)[0] - (int)inputImage.at<Vec3b>(d, j)[0]);
		        int Ygredient = temp * temp;
		        temp = abs((int)inputImage.at<Vec3b>(u, j)[1] - (int)inputImage.at<Vec3b>(d, j)[1]);
		        Ygredient += temp * temp;
		        temp = abs((int)inputImage.at<Vec3b>(u, j)[2] - (int)inputImage.at<Vec3b>(d, j)[2]);
		        Ygredient += temp * temp;

		        int total = Xgredient + Ygredient;

		        energyMatrix[i][j] = sqrt((float)total);
		    }
		}
		
    	//cout<<"inside row seam";
    	float **energycolMatrixDP = new float*[rows];
		for (int i = 0; i < rows; ++i) 
		{
    		energycolMatrixDP[i] = new float[cols];
		}
		
        for (int i = 0; i < cols; i++)
        {
            for (int j = 0; j < rows; j++)
            {
                if(i == 0)
                {
                    energycolMatrixDP[j][i] = energyMatrix[j][i];
                }
                else
                {
                    float lu,l,ld;
                    lu = energycolMatrixDP[(j == 0)?rows-1:j-1][i-1];
                    l  = energycolMatrixDP[j][(i-1)];
                    ld = energycolMatrixDP[(j == rows-1)?0:j+1][i-1];
                                        

                    float min_energy = min(lu, min(l, ld));
                    energycolMatrixDP[j][i] = min_energy + energyMatrix[j][i];
                }
            }
        }
        
		while (expected_rows < rows)
	    {
		    float min_Last_col = INT_MAX; 
		    int min_Last_col_index = INT_MAX; 

		    for (int i = cols - 1; i > 0; i--)
		    {
		        if(i == cols-1)
		        {
		        	for (int i = 0; i < rows; i++)
					{
						if( min_Last_col > energycolMatrixDP[i][cols-1])
						{
						    min_Last_col = energycolMatrixDP[i][cols-1];
						    min_Last_col_index = i;
						}
					}
		            for (int j = min_Last_col_index; j < rows-1; j++)
		            {
		                energyMatrix[j][i] = energyMatrix[j+1][i];
		                inputImage.at<Vec3b>(j, i) = inputImage.at<Vec3b>(j+1, i);
		            }
		        }
		        else
		        {
		            float lu,l,ld;
		            if(min_Last_col_index==0)
		            {
		                lu = INT_MAX;
		                l  = energycolMatrixDP[min_Last_col_index][(i-1)];
		                ld = energycolMatrixDP[(min_Last_col_index == rows-1)?0:min_Last_col_index+1][i-1];
		            }
		            else if( min_Last_col_index == cols-1)
		            {
		                lu = energycolMatrixDP[(min_Last_col_index == 0)?rows-1:min_Last_col_index-1][i-1];
		                l  = energycolMatrixDP[min_Last_col_index][(i-1)];
		                ld = INT_MAX;
		            }
		            else
		            {
		                lu = energycolMatrixDP[(min_Last_col_index == 0)?rows-1:min_Last_col_index-1][i-1];
		                l  = energycolMatrixDP[min_Last_col_index][(i-1)];
		                ld = energycolMatrixDP[(min_Last_col_index == rows-1)?0:min_Last_col_index+1][i-1];
		            }
		            
		            if(lu < l && lu < ld)
		                min_Last_col_index = min_Last_col_index-1;
		            else if(l < lu && l < ld)
		                min_Last_col_index = min_Last_col_index;
		            else 
		                min_Last_col_index = min_Last_col_index+1;

		            for (int j = min_Last_col_index; j < rows-1; j++)
		            {
		                 inputImage.at<Vec3b>(j, i) = inputImage.at<Vec3b>(j+1, i);
		                energycolMatrixDP[j][i] = energycolMatrixDP[j+1][i];
		            }
		    
		        }
		    }
		    rows--;
		}
    }
    Mat outputImage1(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            outputImage1.at<Vec3b>(i, j) = inputImage.at<Vec3b>(i, j);
        }
    }

    imwrite("outputrow1.jpeg", outputImage1);

    return 0;
}
