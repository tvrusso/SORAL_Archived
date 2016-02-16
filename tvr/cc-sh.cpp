// This simplistic hack just prompts for numbers of areas, speed 
#include <string>
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

	int num_areas;
	int num_resources=1;

        cout << " Enter number of areas ";
        cin >> num_areas;

	Array2D effectiveness(num_areas, num_resources);
	valarray<double> availableHours(num_resources);
	valarray<double> POA(num_areas);
	valarray<double> speed(num_areas);
	valarray<double> ESW(num_areas);
	valarray<double> area(num_areas);
        int i;
        string ans;
        bool done;

        cout << "Enter number of searcher-hours available: ";
        cin >> availableHours[0] ;

        for (i=0;i<num_areas;++i)
        {
            cout << endl 
                 << " Enter area in square mi for area number " << i 
                 << ": ";
            cin >> area[i];

            cout << " Enter POA for area number " << i 
                 << ": ";
            cin >> POA[i];

            cout << " Enter effective sweep width (in feet) for area number" << i 
                 << ": ";
            cin >> ESW[i] ;
            ESW[i] /= 5280;  // esw in miles

            cout << " Enter speed for area number in MPH " << i 
                 << ": ";
            cin >> speed[i] ;

        }
            


        for (i=0;i<num_areas;i++)
        {
            effectiveness[i][0]=ESW[i]*speed[i]/area[i];
        }

	// Run the allocation. 
	CharnesCooper theAllocation(num_resources, 
						   num_areas, 
						   effectiveness, 
						   availableHours, 
						   POA);

	// Now display results

	cout << "The calculated allocation\n";
	printAssignments(theAllocation);


        cout << "Coverages: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getCoverage(i) << endl;
        }

        cout << "PODs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getPOD(i) << endl;
        }

        cout << "Adjusted POAs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": Orig: " << POA[i]
                 << " New: " << theAllocation.getNewPOC(i) << endl;
        }

        cout << "PSRs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": Old: " << POA[i]*ESW[i]*speed[i]/area[i]
                 << " New: " 
                 << theAllocation.getNewPOC(i)*ESW[i]*speed[i]/area[i]
                 << endl;
        }
        cout << "Segment POSs: " << endl;
        for (i=0;i<num_areas;i++)
        {
            cout << "  " << i << ": " << theAllocation.getPOS(i) << endl;
        }

        cout << " Cumulative POS: " << theAllocation.getTotalPOS() << endl;

        UserDef changedAllocation(theAllocation);
        done=false;

        while (!done)
        {
            cout << endl
                 << "Do you want to tweak the allocation? " << endl;
            cin >> ans;
            
            done=true;
            if (ans == "yes") 
            {
                done=false;
                ActiveAreasIterator activeItr(changedAllocation);
                
                //While there are still areas with assignments
                while(!(activeItr.atEnd()))
                {
                    ActiveArea area(*activeItr); 
                    int        areaIndex(area.getActiveAreaNum());
                    
                    ResourceIterator resItr(changedAllocation, areaIndex);
                    
                    while (!(resItr.atEnd()))
                    {
                        ResourceAssignment resAssign(*resItr);
                        int resIndex (resAssign.getResourceNum());
                        double time  (resAssign.getTime());
                        
                        cout << "  Area: " << areaIndex 
                             << "  Resource: " << resIndex 
                             << "  Searcher-hours: " << time << endl; 
                        

                        cout << " Change number searcher-hours? " << endl;
                        cin >> ans;
                        if (ans == "yes")
                        {
                            double new_time;
                            cout << " New number searcher-hours: ";
                            cin >> new_time;

                            if (new_time > time)
                                changedAllocation.add(areaIndex,resIndex,(new_time-time));
                            else
                                changedAllocation.remove(areaIndex,resIndex,(time-new_time));                                
                        }
                        ++resItr;
                    }
                    ++activeItr;
                }

                cout << "The calculated allocation\n";
                printAssignments(changedAllocation);
                
                
                cout << "Coverages: " << endl;
                for (i=0;i<num_areas;i++)
                    {
                        cout << "  " << i << ": " 
                             << changedAllocation.getCoverage(i) << endl;
                    }
                
                cout << "PODs: " << endl;
                for (i=0;i<num_areas;i++)
                    {
                        cout << "  " << i << ": " 
                             << changedAllocation.getPOD(i) << endl;
                    }
                
                cout << "Adjusted POAs: " << endl;
                for (i=0;i<num_areas;i++)
                    {
                        cout << "  " << i << ": Orig: " << POA[i]
                             << " New: " 
                             << changedAllocation.getNewPOC(i) << endl;
                    }
                
                cout << "PSRs: " << endl;
                for (i=0;i<num_areas;i++)
                    {
                        cout << "  " << i << ": Old: " 
                             << POA[i]*ESW[i]*speed[i]/area[i]
                             << " New: " 
                             << changedAllocation.getNewPOC(i)*ESW[i]*speed[i]/area[i]
                             << endl;
                    }
                cout << "Segment POSs: " << endl;
                for (i=0;i<num_areas;i++)
                    {
                        cout << "  " << i << ": " 
                             << changedAllocation.getPOS(i) << endl;
                    }
                
                cout << " Cumulative POS: " 
                     << changedAllocation.getTotalPOS() << endl;
            }
        }
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
