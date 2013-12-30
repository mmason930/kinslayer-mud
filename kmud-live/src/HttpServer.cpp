#include "HttpResource.h"
#include "kuSockets.h"
#include "kuListener.h"
#include "kuDescriptor.h"
#include "HttpException.h"

#include "MiscUtil.h"

HttpServer::HttpServer()
{
	topRequestId = 0;
	setQueue(new HttpQueue());
}

HttpQueue *HttpServer::getQueue()
{
	return queue;
}

void HttpServer::setQueue(HttpQueue *queue)
{
	this->queue = queue;
}

int HttpServer::getPort()
{
	return port;
}

void HttpServer::deploy(int port)
{
	this->port = port;

	mainThread = std::thread( &HttpServer::run, this );

	mainThread.detach();
}

void httpServerOnCloseDescriptorCallback(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	HttpServer *httpServer = (HttpServer*)(data);
	HttpDescriptor *httpDescriptor = httpServer->getHttpDescriptor(descriptor);

	std::cout << "[" << descriptor->getUid() << "] Removing Descriptor...";

	if(httpDescriptor != NULL)
	{
		std::cout << "HTTP Descriptor Found" << std::endl;
		httpServer->removeHttpDescriptor(httpDescriptor);
		delete httpDescriptor;
	}
	else
		std::cout << "HTTP Descriptor NOT FOUND!" << std::endl;
}

void HttpServer::removeHttpDescriptor(HttpDescriptor *httpDescriptor)
{
	this->httpDescriptors.remove(httpDescriptor);
}

void HttpServer::run()
{
	kuListener *listener = new kuListener(port, TCP);
	listener->setCloseDescriptorCallback(httpServerOnCloseDescriptorCallback);
	listener->setDataForCloseDescriptorCallback(this);
	
	bool running = true;
	while(running)
	{
		std::list<kuDescriptor *> incomingDescriptors = listener->l_AcceptNewHosts();
		
		//Handle the incoming descriptors.
		for(auto descriptorIter = incomingDescriptors.begin();descriptorIter != incomingDescriptors.end();++descriptorIter)
		{
			HttpDescriptor *descriptor = new HttpDescriptor( (*descriptorIter), this );
			this->httpDescriptors.push_back(descriptor);

			std::cout << "[" << (*descriptorIter)->getUid() << "] Descriptor Added" << std::endl;
		}

		listener->l_Pulse();

		std::list<kuDescriptor *> kuDescriptorList = listener->l_GetDescriptors();
		for(auto descriptorIter = kuDescriptorList.begin();descriptorIter != kuDescriptorList.end();++descriptorIter)
		{
			kuDescriptor *descriptor = (*descriptorIter);

			if(descriptor->socketIsClosed())
				continue;

			try
			{
				HttpDescriptor *httpDescriptor = getHttpDescriptor(descriptor);

				if(!httpDescriptor)
				{
					std::cout << "[" << descriptor->getUid() << "] NO HTTP DESCRIPTOR FOUND" << std::endl;
				}

				if(httpDescriptor->isReading())
				{
					std::string inputBuffer = descriptor->getInputBuffer();
					httpDescriptor->read(inputBuffer);
					descriptor->clearInput();

					if(httpDescriptor->isReading() == false)
					{
						std::cout << "Putting http request into queue." << std::endl;
						queue->putRequest(httpDescriptor->getRequest());
					}
				}
			}
			catch(HttpException ex)
			{
				std::cout << "EXCEPTION THROWN: " << ex.what() << std::endl;
				descriptor->disconnect();
			}
		}


		HttpResponse *httpResponse;
		while( (httpResponse = queue->getNextResponse()) != NULL )
		{
			HttpDescriptor *httpDescriptor = this->getDescriptorByRequestId(httpResponse->getId());

			if(httpDescriptor != NULL)
			{
				httpDescriptor->getKuDescriptor()->socketWriteInstant( httpResponse->generatePacket() );
				httpDescriptor->reset();
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds(50) );
	}

	delete listener;
}

HttpDescriptor *HttpServer::getHttpDescriptor(kuDescriptor *kuDescriptor)
{
	for(auto iter = httpDescriptors.begin();iter != httpDescriptors.end();++iter)
	{
		HttpDescriptor *httpDescriptor = (*iter);
		if(httpDescriptor->getKuDescriptor() == kuDescriptor)
			return httpDescriptor;
	}

	return NULL;
}

void HttpServer::disconnect()
{
	//TODO: Finish
}

void HttpServer::addResource(HttpResource *resource)
{
	{
		std::lock_guard<std::mutex> lock(this->resourceMutex);
		resources[ resource->getPath() ] = resource;
	}
}

unsigned int HttpServer::getNextRequestId()
{
	return topRequestId++;
}

HttpDescriptor *HttpServer::getDescriptorByRequestId(unsigned int requestId)
{
	for(auto iter = httpDescriptors.begin();iter != httpDescriptors.end();++iter)
	{
		HttpDescriptor *httpDescriptor = (*iter);
		if(httpDescriptor->getRequest() && httpDescriptor->getRequest()->getId() == requestId)
			return httpDescriptor;
	}

	return NULL;
}

bool HttpServer::hasResource(const std::string &resourceName) const
{
	return resources.find(resourceName) != resources.end();
}

HttpResource *HttpServer::getResource(const std::string &resourceName) const
{
	if(!hasResource(resourceName))
		throw HttpException( (std::string("Resource `") + resourceName + std::string("` not found")).c_str() );
	auto iter = resources.find(resourceName);
	return (*iter).second;
}
