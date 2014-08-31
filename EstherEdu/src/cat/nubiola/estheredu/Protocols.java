package cat.nubiola.estheredu;

import java.util.ArrayList;
import java.util.List;

public enum Protocols {
	http,
	https,
	fttp,
	fttps,
	sftp,
	eshteredu;
	
	static List<String>getList(){
		ArrayList<String> ret = new ArrayList<String>();
		for( Protocols prot : values()  ){
			ret.add(prot.name());
		}
		return ret;
	}
}
