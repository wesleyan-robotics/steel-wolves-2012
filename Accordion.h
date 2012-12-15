#ifndef ACCORDION_H
#define ACCORDION_H

const int ACCORDION_SPEED_UP   = 60;
const int ACCORDION_SPEED_DOWN = 30;
const float ACCORDION_RATIO = 3.3921; /* Ratio between rotations from the small gear to the bigger gear */

int BottomAccordion = -1;
int TopAccordion = -1;

void initAccordion(int bottom, int top)
{
	BottomAccordion = bottom;
	TopAccordion = top;
}

void raiseAccordion()
{
	motor[AccordionTop] = -ACCORDION_SPEED_UP;
	motor[AccordionBottom] = -ACCORDION_SPEED_UP / ACCORDION_RATIO;
}

void lowerAccordion()
{
	motor[AccordionTop] = ACCORDION_SPEED_DOWN;
	motor[AccordionBottom] = ACCORDION_SPEED_DOWN / ACCORDION_RATIO;
}

void stopAccordion()
{
	motor[AccordionTop] = 0;
	motor[AccordionBottom] = 0;
}

#endif
