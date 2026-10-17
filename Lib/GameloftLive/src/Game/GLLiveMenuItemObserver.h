#ifndef __MENU_ITEM_OBSERVER_H__
#define __MENU_ITEM_OBSERVER_H__

/**
 * Is is designed for handling the OnClicked event of menu item.
 * <p>
 *
 * @version 0.0.1
 *
 */

class MenuItemObserver1
{
public:
	
	virtual void OnMenuItemClicked( int id ) = 0;

};

#endif // __MENU_ITEM_OBSERVER_H__