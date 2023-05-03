
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "Dip.h"
#include "DipSubscription.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define CONVERT_LONGS_TO_INTS


using namespace std;

//How long the test will run in ms. Default is 1 minute.


/**
* Simple demo client that subscribes to data coming from servers.
* Subscribe to "dip/test/api/dip-server_<from 0 to noPubs>"
*/

class Dumper 
{
private:
	char 	FNPrefix[128];
	int	CloseInterval;

	FILE* 	CurrentFile;
	char 	CurrentFileName[128];
	double 	CurrentOpenTime;
	double	EventBegin;
	float	EventWindow;

public:
	Dumper(const char * fnprefix, int close_interval)
	: EventWindow(1.0)		// 1 second
	{
		strcpy(FNPrefix, fnprefix);
		CloseInterval = close_interval;
		CurrentFile = NULL;
		CurrentOpenTime = 0.0;
		EventBegin = 0.0;
	}

	~Dumper()
	{
		closeCurrentFile();
	}

private:

	void closeCurrentFile()
	{
		if( CurrentFile != NULL )
		{
			char new_file_name[128];
			sprintf(new_file_name, "%s.closed", CurrentFileName);
                	fclose(CurrentFile);
			rename(CurrentFileName, new_file_name);
		}
		CurrentFile = NULL;
	}

	void reopenFile(double t)
	{
		if( t >= CurrentOpenTime + CloseInterval )
		{
			closeCurrentFile();
			sprintf(CurrentFileName, "%s/data_%d_%.3f.data", FNPrefix, getpid(), t);
			CurrentFile = fopen(CurrentFileName, "w");
			if( CurrentFile == NULL )
			{
				cout << "Can not open file " << CurrentFileName << endl;
				exit(1);
			}
			printf("file %s opened\n", CurrentFileName);
			CurrentOpenTime = t;
			EventBegin = 0.0;
		}
	}

	void reopenEvent(double t)
	{
		if( t >= EventBegin + EventWindow )
		{
			long tl = (long)(t*1000.0);		//miliseconds
			fprintf(CurrentFile, "*z,pdune %ld 0\n", tl);
			EventBegin = t;
			//printf("reopening event with t=%f...\n", EventBegin);
		}
		//else
		//	//printf("continue event, event time:%f, t:%f\n", EventBegin, t);
	}

	inline double getTimeStamp(const DipData &message)
	{
		superlong tl = message.extractDipTime().getAsNanos();
		return (double)tl*1.0e-9;
	}

	std::string dumpItem(const char *topic, const DipData &message, const char* tag)
	{
		// IFBeam compatible version
		ostringstream oss;
		
		DipDataType type = message.getValueType(tag);

		//if( type == TYPE_STRING || type == TYPE_STRING_ARRAY )	// IFBeam can not handle strings
		//	return std::string("");

		long tl = getTimeStamp(message)*1000.0;	// time as miliseconds
		const DipBool *ba;
		const DipLong *dl;
		const DipInt *di;
		const DipShort *ds;
		const DipByte *ta;
		const DipFloat *fa;
		const DipDouble *da;
		unsigned long lv;
		const char **dt;
		int i, n;
		long packed, j;
		int first = 1;
		int is_array = isArrayType(message, tag);
#ifdef CONVERT_LONGS_TO_INTS
		is_array = is_array || (type == TYPE_LONG);
#endif		
		oss << topic << ":" << tag;
		if( is_array )
			oss << "[]";
		oss << "\t" << tl << "\t" << "null" << "\t";
		if ( is_array )
			oss << "null\t{";
		
		switch(type)
		{
			case TYPE_BOOLEAN:
				oss << (message.extractBool(tag) ? 1 : 0);
				break;

			case TYPE_BOOLEAN_ARRAY:
				ba = message.extractBoolArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << (ba[i] ? 1 : 0) ;
				break;
#ifdef CONVERT_LONGS_TO_INTS
			case TYPE_LONG:
				lv = message.extractLong(tag);
				oss << ((lv >> 32) & 0xFFFFFFFF) << ',' << (lv & 0xFFFFFFFF);
				break;
#else
			case TYPE_LONG:
				oss << message.extractLong(tag);
				break;
#endif

#ifdef CONVERT_LONGS_TO_INTS
			case TYPE_LONG_ARRAY:
				dl = message.extractLongArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << ((dl[i]>>32) & 0xFFFFFFFF) << ',' << (dl[i] & 0xFFFFFFFF);
				break;
#else
			case TYPE_LONG_ARRAY:
				dl = message.extractLongArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << dl[i];
				break;
#endif

			case TYPE_INT:
				oss << message.extractInt(tag);
				break;
			case TYPE_INT_ARRAY:
				di = message.extractIntArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << di[i];
				break;

			case TYPE_SHORT:
				oss << message.extractShort(tag);
				break;
			case TYPE_SHORT_ARRAY:
				ds = message.extractShortArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << ds[i];
				break;

			case TYPE_BYTE:
				oss << (unsigned int)message.extractByte(tag);
				break;
			case TYPE_BYTE_ARRAY:
					ta = message.extractByteArray(n, tag);
					packed = 0;
					j = 0;
					for( i=0; i<n; i++ )
					{
						packed |= (ta[i] << (8*j));
						j += 1;
						if( j >= 4 || n == n-1 )
						{
							oss << (first ? "" : ",") << packed;
							j = 0;
							packed = 0;
							first = 0;
						}
					}
				break;

			case TYPE_FLOAT:
				oss << message.extractFloat(tag);
				break;
			case TYPE_FLOAT_ARRAY:
				fa = message.extractFloatArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << fa[i];
				break;

			case TYPE_DOUBLE:
				oss << message.extractDouble(tag);
				break;
			case TYPE_DOUBLE_ARRAY:
				da = message.extractDoubleArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << da[i];
				break;

			case TYPE_STRING:
				oss << '"' << message.extractCString(tag) << '"';
				break;
			case TYPE_STRING_ARRAY:
				dt = message.extractCStringArray(n, tag);
				for( i=0; i<n; i++ )
					oss << (i==0 ? "" : ",") << '"' << dt[i] << '"';
				break;

			case TYPE_NULL:
				oss << "null";
				break;
		}
		if ( is_array )
			oss << "}";
		else
			oss << "\tnull";
		return oss.str();
	}

	inline int isArrayType(const DipData &message, const char* tag)
	{
		return message.getValueType(tag) >= 10;
	}

public:
	void dataReceived(const char *topic, DipData &message)
	{
		//cout << "dataReceived:" << topic << endl;
		double t = getTimeStamp(message);

		//printf("dataReceived(): t=%f topic=%s\n", t, topic);

		reopenFile(t);
		reopenEvent(t);

		int ntags;
		const char ** tags = message.getTags(ntags);
		int i;
		//fprintf(CurrentFile, "t\t%.3f\t%s\n", t, topic);
		//
		for( i=0; i<ntags; i++ )
		{
//#define REPEAT_LINES 10
#ifdef REPEAT_LINES
			int r;
			for( r=0; r<REPEAT_LINES; r++ )
			{
				char topic_repeated[128];
				sprintf(topic_repeated, "%s_%d", topic, r);
				std::string line = dumpItem(topic_repeated, message, tags[i]);
				if( line.length() > 0 )
					fprintf(CurrentFile, "%s\n", line.c_str());
			}
#else
			std::string line = dumpItem(topic, message, tags[i]);
			if( line.length() > 0 )
				fprintf(CurrentFile, "%s\n", line.c_str());
#endif
		}
		//cout << "dataReceived:" << topic << " done" << endl;
	}

	void flush()
	{
		closeCurrentFile();
	}
};

class Client:public DipSubscriptionListener {

private:
	// hold reference to subscription objects
	DipSubscription **sub;
	int nsubs;

	// DIP object
	DipFactory *dip;
	
	Dumper *dumper;

public:
	Client(Dumper *d, int nvars, const char **vars)
	: dumper(d)
	{
		nsubs = nvars;

		dip = Dip::create("dip-client");
		//Creating an array of DipSubscriptions.
		sub = new DipSubscription*[nvars];
		//dip->setDNSNode("dipnsgpn1");
		int i;
		for( i=0; i<nvars; i++ )
		{	//cout << "subscribing to " << vars[i] << " ...";
		 	sub[i] = dip->createDipSubscription(vars[i], this);
			//cout << "subscribed" << endl;
		}
	}

	~Client()
	{
		int i;
		for( i=0; i<nsubs; i++ )
		 	dip->destroyDipSubscription(sub[i]);
	  	delete dip;
	  	cout<<"Client's done."<<endl;
	}

	/**
	* handle changes to subscribed to publications
	* Simply prints the contents of the received data.
	* @param subscription - the subsciption to the publications thats changed.
	* @param message - object containing publication data
	* */

#define MAXTAGS 300
	void handleMessage(DipSubscription *subscription, DipData &message)
	{
		dumper->dataReceived(subscription->getTopicName(), message);
	}
			

	/**
	* called when a publication subscribed to is available.
	* @param arg0 - the subsctiption who's publication is available.
	* */
	void connected(DipSubscription *arg0) {
		cout << "\nPublication source  " << arg0->getTopicName()<< " available\n";
	}


	/**
	* called when a publication subscribed to is unavailable.
	* @param arg0 - the subsctiption who's publication is unavailable.
	* @param arg1 - string providing more information about why the publication is unavailable.
	* */
	void disconnected(DipSubscription *arg0, char *arg1) {
		//printf("\nPublication source %s unavailable\n", arg0->getTopicName());
	}

	void handleException(DipSubscription* subscription, DipException& ex){
		//printf("Subs %s has error %s\n", subscription->getTopicName(), ex.what());
	}


	/**
	* set up the subscriptions to 
	* */
};

#if 0

class NameTranslator
{
	int NTranslations;
	const char **Translations;
	char **Words;

public:
	NameTranslator(int nnames, char** names)
	{
		// names are: "<DIP name>:<ACNen name>"
		Translations = new (const char*)[nnames*2];
		Words = new (char*)[nnames];
		NTranslations = nnames;
		int i;
		for(i=0; i<nnames; i++)
		{
			char *w = strdup(names[i]);
			char *dip_name = strtok(w, ":");
			char *acnet_name = strtok(NULL, ":");
			Translations[i*2] = dip_name;
			Translations[i*2+1] = acnet_name;
			Words[i] = w;
		}
	}
	~NameTranslator()
	{
		int i;
		for(i=0; i<NTranslations; i++)
			free(Words[i]);
		delete Translations;
		delete Words;
	}

	const char* translate(const char* dip_name)
	{
		int i;
		for(i=0; i<NTranslations; i++)
			if( strcmp(dip_name, Translations[i*2]) == 0 )
				return Translations[i*2+1];
		return NULL;
	}
}

#endif


/**
* Start the client
* */
int main(const int argc, const char ** argv)
{
	const char *usage = "client [-t <lifetime, min>: 90minutes] [-f <file close interval, min>: 1 minute] <output_dir> <subscription> ...";

	int lifetime = 120*60; // 2 hours
	int file_close_interval = 60;

	int i;
	for( i=1; i<argc; i++ )
	{
		const char *arg = argv[i];
		if( arg[0] == '-' )
		{
			switch(arg[1])
			{
				case 't':
					lifetime = atoi(argv[++i])*60;
					break;
				case 'f':
					file_close_interval = atoi(argv[++i])*60;
					break;
			}
		}
		else if( i > argc - 2 )
		{
			printf(usage);
			return 1;
		}
		else
			break;
	}

	if( i >= argc )
	{
		printf(usage);
		return 1;
	}

	Dumper *dumper = new Dumper(argv[i], file_close_interval);
	printf("subscriptions: %d\n", argc-i-1);
	Client *client = new Client(dumper, argc-i-1, argv+i+1);

	sleep(lifetime);
	cout<<"Client's lifetime has expired, leaving... "<<endl;

	delete client;
	dumper->flush();
	delete dumper;
	return(0);
}

