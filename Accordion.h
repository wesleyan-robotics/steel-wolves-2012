#ifndef ACCORDION_H
#define ACCORDION_H

const int ACCORDION_SPEED_UP   =  60;
const int ACCORDION_SPEED_DOWN = -25;
const int ACCORDION_LOW_SPEED_DOWN = -18; /* TODO: These Values need to be calibrated with the battry charged */
const int ACCORDION_LOW_SPEED_UP  =   45; /* TODO: Including this one for raising and lowering to the peg programmically */
const float ACCORDION_RATIO = 3.3921; /* Ratio between rotations from the small gear to the bigger gear */

int BottomAccordion = -1;
int TopAccordion = -1;

void initAccordion(int bottom, int top)
{
	BottomAccordion = bottom;
	TopAccordion = top;
}

void setAccordion(int speed)
{
	motor[AccordionTop] = speed;
	motor[AccordionBottom] = speed / ACCORDION_RATIO;
}

void raiseAccordion()
{
	setAccordion(ACCORDION_SPEED_UP);
}

void lowerAccordion()
{
	setAccordion(ACCORDION_SPEED_DOWN);
}

void stopAccordion()
{
	setAccordion(0);
}

#endif
