/*
 * MIT License
 *
 * Copyright (c) 2019 Linas Nikiperavicius
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef OSSHS_PROTOCOL_USART_INTERFACE_HPP
	#error "Don't include this file directly, use 'usart_interface.hpp' instead!"
#endif

#include <modm/platform.hpp>
#include <osshs/resource_lock.hpp>
#include <osshs/log/logger.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			namespace usart
			{
				template<typename USART>
				bool
				UsartInterface<USART>::run()
				{
					PT_BEGIN();

					do
					{
						PT_WAIT_WHILE(eventPacketQueue.empty());

						currentEventPacket = eventPacketQueue.front();
						eventPacketQueue.pop();

						PT_CALL(writeEventPacket(currentEventPacket));

						currentEventPacket.reset();

						PT_YIELD();
					}
					while (true);

					PT_END();
				}

				template<typename USART>
				void
				UsartInterface<USART>::initialize()
				{
					OSSHS_LOG_INFO("Initializing USART interface.");
				}

				template<typename USART>
				modm::ResumableResult<void>
				UsartInterface<USART>::writeEventPacket(std::shared_ptr<EventPacket> eventPacket)
				{
					RF_BEGIN();

					OSSHS_LOG_DEBUG(
						"Writing event packet(multiTarget = %u, command = %u, transmitterMac = 0x%08x, receiverMac = 0x%08x, eventType = 0x%04x).",
						eventPacket->isMultiTarget(),
						eventPacket->isCommand(),
						eventPacket->getTransmitterMac(),
						eventPacket->getReceiverMac(),
						eventPacket->getEvent()->getType()
					);

					RF_WAIT_UNTIL(ResourceLock<USART>::tryLock());

					{
						std::unique_ptr<const uint8_t[]> buffer = eventPacket->serialize();

						if (buffer == nullptr)
						{
							OSSHS_LOG_WARNING("Failed to serialize event packet.");
							RF_RETURN();
						}

						uint16_t bufferLength = buffer[0] | (buffer[1] << 8);

						USART::writeBlocking(buffer.get(), bufferLength);
					}

					ResourceLock<USART>::unlock();

					RF_END();
				}
			}
		}
	}
}
