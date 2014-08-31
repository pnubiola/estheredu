package cat.nubiola.estheredu;

import java.net.URLStreamHandler;
import java.net.URLStreamHandlerFactory;

public class EstherEduURLStreamHandlerFactory implements
		URLStreamHandlerFactory {

	@Override
	public URLStreamHandler createURLStreamHandler(String protocol) {
		if ( protocol.equalsIgnoreCase("estheredu") ) 
            return new EstherEduURLStreamHandler(); 
        else 
            return null; 
	}

}
