/*
  Copyright (c) 2006-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "../../../../include/chat/gloox/stanzaextensionfactory.h"

#include "../../../../include/chat/gloox/gloox.h"
#include "../../../../include/chat/gloox/tag.h"
#include "../../../../include/chat/gloox/oob.h"
#include "../../../../include/chat/gloox/vcardupdate.h"
#include "../../../../include/chat/gloox/delayeddelivery.h"
#include "../../../../include/chat/gloox/xdelayeddelivery.h"
#include "../../../../include/chat/gloox/gpgsigned.h"
#include "../../../../include/chat/gloox/gpgencrypted.h"

namespace gloox
{

  StanzaExtension* StanzaExtensionFactory::create( Tag* tag )
  {
    const std::string& name = tag->name();
    const std::string& xmlns = tag->findAttribute( "xmlns" );
    if( name == "x" )
    {
      if( xmlns == XMLNS_X_DELAY )
        return new XDelayedDelivery( tag );
      else if( xmlns == XMLNS_X_OOB )
        return new OOB( tag );
      else if( xmlns == XMLNS_X_VCARD_UPDATE )
        return new VCardUpdate( tag );
      else if( xmlns == XMLNS_X_GPGSIGNED )
        return new GPGSigned( tag );
      else if( xmlns == XMLNS_X_GPGENCRYPTED )
        return new GPGEncrypted( tag );
    }
    else if( name == "iq" )
    {
      if( xmlns == XMLNS_IQ_OOB )
        return new OOB( tag );
    }
    else if( name == "delay" && xmlns == XMLNS_DELAY )
      return new DelayedDelivery( tag );

    return 0;
  }

}
