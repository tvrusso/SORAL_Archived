/*********************************************************************
 *      SARBayes OPTIMAL RESOURCE ALLOCATION LIBRARY 2001-03         *
 *                                                                   *
 *********************************************************************/
/**
 *  \file sample.cpp
 *  \brief This is a simple sample of how to use SORAL.
 *
 * This program creates an allocation, and then prints it by walking
 * over the results.
 *
 * <b>Version History</b>
 *
 * \verbatim
 *-----+----------+-----+-----------------------------------------------------
 * Who |   When   | Ver | What
 *-----+----------+-----+-----------------------------------------------------
 * ASO | 18/1/03  |  1  | Created as TestBed.cpp
 *-----+----------+-----+-----------------------------------------------------
 * crt |  7apr03  |  2  | Made sample.cpp (TestBed is now for real testing).
 *-----+----------+-----+-----------------------------------------------------
 * crt | 03may03  |  3  | Simplified printAssignments.
 *-----+----------+-----+-----------------------------------------------------
 * \endverbatim
 */

//===========================================================================//
// Written by Andre Oboler                               http://sarbayes.org //
//---------------------------------------------------------------------------//
// The SORAL implementation is free software, but it is Copyright (C)        //
// 2001-2003 the authors and Monash University (the SARBayes project).       //
// It is distributed under the terms of the GNU General Public License.      //
// See the file COPYING for copying permission.                              //
//                                                                           //
// If those licencing arrangements are not satisfactory, please contact us!  //
// We are willing to offer alternative arrangements, if the need should arise//
//                                                                           //
// THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED OR //
// IMPLIED.  ANY USE IS AT YOUR OWN RISK.                                    //
//                                                                           //
// Permission is hereby granted to use or copy this program for any purpose, //
// provided the above notices are retained on all copies.  Permission to     //
// modify the code and to distribute modified code is granted, provided the  //
// above notices are retained, in accordance with the GNU GPL.               //
//===========================================================================//

#include <iostream>
#include <vector>
#include "containr.h"
#include "Allocatn.h"
#include "Array2D.h"
#include "Alloc-W.h"
#include "Alloc-CC.h"
#include "UserDef.h"



using namespace std;

// function definitions
void printAssignments(const Allocation& theAllocation);

//int main(int argc, int argv[]) // crt: we're not using args
int main()
{

	int num_areas=4;
	int num_resources=1;
	Array2D effectiveness(num_areas, num_resources);
	valarray<double> availableHours(num_resources);
	valarray<double> POA(num_areas);
	valarray<double> speed(num_areas);
	valarray<double> ESW(num_areas);
	valarray<double> area(num_areas);
        int i;

        // Area 1
        area[0]=28;
        POA[0] = 0.1935;
        ESW[0] = 1;
        speed[0] = 1;

        // Area 2
        area[1]=30;
        POA[1] = 0.2581;
        ESW[1] = 1;
        speed[1] = 1;

        // Area 3
        area[2]=14;
        POA[2] = 0.2903;
        ESW[2] = 1;
        speed[2] = 1;

        // Area 4
        area[3]=12;
        POA[3] = 0.2581;
        ESW[3] = 1;
        speed[3] = 1;

        for (i=0;i<num_areas;i++)
        {
            effectiveness[i][0]=ESW[i]*speed[i]/area[i];
        }

	availableHours[0]=40;

	// Run the allocation. 
	CharnesCooper theAllocation(num_resources, 
						   num_areas, 
						   effectiveness, 
						   availableHours, 
						   POA);

	// Now display results

	cout << "The calculated allocation\n";
	printAssignments(theAllocation);


        cout << "PODs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getPOD(i) << endl;
        }

        cout << "Adjusted POAs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getNewPOC(i) << endl;
        }
        cout << "Segment POSs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getPOS(i) << endl;
        }

        cout << " Cumulative POS: " << theAllocation.getTotalPOS() << endl;


	return 1;
}

/**** printAssignments() *************************************************/
/// Prints the assignments in an Allocation.
/**
 * Prints all and only the assignments in an Allocation, by using
 * the standard iterator paradigm: start with ActiveAreasIterator
 * and proceed from there.
 *
 * Since this program now compares answers numerically, and only
 * prints the offending allocations, maybe we can remove this
 * function, which now lives usefully in sample.cpp?
 * \todo Remove printAssignments from here?
 *
 * Author: Andre Oboler
 * Simplified: crt 01may03
 */

void printAssignments(const Allocation& theAllocation)
{

	ActiveAreasIterator activeItr(theAllocation);

	//While there are still areas with assignments
	while(!(activeItr.atEnd()))
	{
	    ActiveArea area(*activeItr); 
	    int        areaIndex(area.getActiveAreaNum());

		ResourceIterator resItr(theAllocation, areaIndex);
		
		while (!(resItr.atEnd()))
		{
		  ResourceAssignment resAssign(*resItr);
		  int resIndex (resAssign.getResourceNum());
		  double time  (resAssign.getTime());

		  cout << "  Area: " << areaIndex 
			   << "  Resource: " << resIndex 
			   << "  Time: " << time << endl;		  

		  ++resItr;
		}

		++activeItr;
	}

}
